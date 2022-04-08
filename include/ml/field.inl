
template<unsigned N, unsigned M, unsigned V, typename F>
Field<N, M, V, F>::Field(
        std::vector<float> &source,
        std::vector<float> &destination,
        std::array<float, V> initial) :
    Layer(source, destination),
    variables(source.size(), initial),
    changes(source.size())
{
    for(auto &i : changes) i = {};
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::push(){
    for(unsigned i=0, j=0, k=0; i<left.size(); i+=N, j+=M, k++)
        F::f(left.data()+i, right.data()+j, (variables.data()+k)->data());
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::pull(){
    for(unsigned i=0, j=0, k=0; i<left.size(); i+=N, j+=M, k++)
        F::df(left.data()+i, right.data()+j,
                (variables.data()+k)->data(),
                (changes.data()+k)->data());
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::change(double factor){
    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<V; j++){
            variables[i][j] += factor * changes[i][j];
            changes[i][j] = 0.0f;
        }
    }
}

template<unsigned N, unsigned M, unsigned V, typename F>
bool Field<N, M, V, F>::ok(std::vector<float> &left, std::vector<float> &right){
    return left.size() * M == right.size() * N;
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::save(ui::Saver &saver){
    for(auto &i : variables) for(float j : i) saver.write_float(j);
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::load(ui::Loader &loader){
    for(auto &i : variables) for(float &j : i) j = loader.read_float();
}

