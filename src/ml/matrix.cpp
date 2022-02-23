#include "ml/matrix.h"
#include "ml/util.h"

#include <assert.h>

namespace ml {

Matrix::Matrix(
        std::vector<std::complex<float> > &source,
        std::vector<std::complex<float> > &destination) :
    Layer(source, destination)
{
    matrix.resize(right.size(), std::vector<std::complex<float> >(left.size()));
    changes.resize(right.size(), std::vector<std::complex<float> >(left.size(), 0.0f));
    for(auto &i : matrix) for(auto &j : i) j = random_complex();
}

void Matrix::push(){
    
    for(auto &i : right) i = 0.0f;

    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<right.size(); j++){
            right[j] += left[i] * matrix[i][j];
        }
    }
}

void Matrix::pull(){

    for(auto &i : left) i = 0.0f;

    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<right.size(); j++){
            left[i] += right[j] * matrix[i][j];
            changes[i][j] += left[i] * right[j];
        }
    }
}

void Matrix::change(double factor){
    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<right.size(); j++){
            matrix[i][j] += changes[i][j] * (float)factor;
        }
    }
}

void Matrix::save(app::Saver &saver){
    
    saver.write_unsigned(left.size());
    saver.write_unsigned(right.size());
    for(auto &i : matrix) for(auto &j : i) saver.write_complex(j);

}

void Matrix::load(app::Loader &loader){

    assert(loader.read_unsigned() == left.size());
    assert(loader.read_unsigned() == right.size());
    for(auto &i : matrix) for(auto &j : i) j = loader.read_complex();

}

std::string Matrix::get_type(){ return type; };

const std::string Matrix::type = "matrix";

}

