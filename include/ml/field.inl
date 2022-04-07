
template<unsigned N, unsigned M, unsigned V, typename F>
Field<N, M, V, F>::Field(std::vector<float> &source, std::vector<float> &destination) :
    Layer(source, destination),
    var(source.size())
{
    for(auto &i : var)
        for(unsigned j=0; j<V; j++)
            i[j] = 2.0f * (float)rand() / RAND_MAX - 1.0f;
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::push(){
    for(unsigned i=0, j=0; i<left.size(); i+=N, j+=M) F::f(left.data()+i, right.data()+j);
}

template<unsigned N, unsigned M, unsigned V, typename F>
void Field<N, M, V, F>::pull(){
    for(unsigned i=0, j=0; i<left.size(); i+=N, j+=M) F::df(right.data()+j, left.data()+i);
}

template<unsigned N, unsigned M, unsigned V, typename F>
bool Field<N, M, V, F>::ok(std::vector<float> &left, std::vector<float> &right){
    return left.size() * M == right.size() * N;
}
