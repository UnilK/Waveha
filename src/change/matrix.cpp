#include "change/matrix.h"
#include "math/constants.h"

#include <assert.h>
#include <stdlib.h>

namespace change {

using std::vector;
using std::complex;
typedef complex<float> cfloat;

Matrix::Matrix(unsigned size){
    resize(size);
}

Matrix::Matrix(){}

void Matrix::resize(unsigned size){
    x.resize(size);
    for(auto &i : x) i.resize(size);
    set_unit();
}

unsigned Matrix::size() const {
    return x.size();
}

cfloat Matrix::operator()(unsigned r, unsigned c) const {
    assert(r < size() && c < size());
    return x[r][c];
}

vector<cfloat> Matrix::operator*(const vector<cfloat> &vec) const {
    
    assert(vec.size() == size());

    vector<cfloat> res(size(), 0.0f);

    for(unsigned i=0; i<size(); i++){
        for(unsigned j=0; j<size(); j++){
            res[i] += vec[j] * x[i][j];
        }
    }

    return res;
}

void Matrix::set(unsigned r, unsigned c, cfloat m){
    assert(r < size() && c < size());
    x[r][c] = m;
}

void Matrix::add(unsigned r, unsigned c, cfloat m){
    assert(r < size() && c < size());
    x[r][c] += m;
}

void Matrix::multiply(unsigned r, unsigned c, cfloat m){
    assert(r < size() && c < size());
    x[r][c] *= m;
}

void Matrix::set_unit(){
    for(unsigned i=0; i<size(); i++){
        for(unsigned j=0; j<size(); j++){
            x[i][j] = i == j ? 1.0f : 0.0f;
        }
    }
}

void Matrix::set_nil(){
    for(unsigned i=0; i<size(); i++){
        for(unsigned j=0; j<size(); j++){
            x[i][j] = 0.0f;
        }
    }
}

void Matrix::phase_shuffle(){
    for(unsigned i=0; i<size(); i++){
        for(unsigned j=0; j<size(); j++){
            x[i][j] *= std::polar(1.0f, (float)rand()/RAND_MAX*2*PIF);
        }
    }
}

}

