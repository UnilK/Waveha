
template<unsigned N, unsigned M, unsigned V, typename F>
Field<N, M, V, F>::Field(arrays in, arrays out, args a,
        std::array<float, V> initial) :
    Layer(in, out, a), l(left[0]), r(right[0]),
    variables(l.size, initial),
    changes(l.size)
{
    for(auto &i : changes) i = {};
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::push(){
    for(unsigned i=0, j=0, k=0; i<l.size; i+=N, j+=M, k++)
        F::f(l.data+i, r.data+j, (variables.data()+k)->data());
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::pull(){
    for(unsigned i=0, j=0, k=0; i<l.size; i+=N, j+=M, k++)
        F::df(l.data+i, r.data+j,
                (variables.data()+k)->data(),
                (changes.data()+k)->data());
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::change(double factor){
    for(unsigned i=0; i<l.size; i++){
        for(unsigned j=0; j<V; j++){
            variables[i][j] += factor * changes[i][j];
            changes[i][j] = 0.0f;
        }
    }
}

template<unsigned N, unsigned M, unsigned V, typename F>
bool Field<N, M, V, F>::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size * M == out[0].size * N && in[0].size > 0;
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::save(ui::Saver &saver){
    for(auto &i : variables) for(float j : i) saver.write_float(j);
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::load(ui::Loader &loader){
    for(auto &i : variables) for(float &j : i) j = loader.read_float();
}

