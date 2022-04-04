#include "ml/matrix.h"
#include "ml/util.h"

#include <assert.h>

namespace ml {

Matrix::Matrix(
        std::vector<float> &source,
        std::vector<float> &destination) :
    Layer(source, destination)
{
    matrix.resize(left.size(), std::vector<float>(right.size()));
    changes.resize(left.size(), std::vector<float>(right.size(), 0.0f));
    for(auto &i : matrix) for(auto &j : i) j = random_float();
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
            changes[i][j] = 0.0f;
        }
    }
}

void Matrix::save(ui::Saver &saver){
    
    saver.write_unsigned(left.size());
    saver.write_unsigned(right.size());
    for(auto &i : matrix) for(auto &j : i) saver.write_float(j);

}

void Matrix::load(ui::Loader &loader){

    assert(loader.read_unsigned() == left.size());
    assert(loader.read_unsigned() == right.size());
    for(auto &i : matrix) for(auto &j : i) j = loader.read_float();

}

namespace Factory { extern std::string matrix; }
std::string Matrix::get_type(){ return Factory::matrix; };

}

