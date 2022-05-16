#include "ml/matrix.h"
#include "ml/util.h"

#include <assert.h>

namespace ml {

// matrix /////////////////////////////////////////////////////////////////////

Matrix::Matrix(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]),
    matrix(l.size, std::vector<float>(r.size)),
    changes(l.size, std::vector<float>(r.size, 0.0f))
{
    for(auto &i : matrix) for(auto &j : i) j = random_float();
}

void Matrix::push(){
    
    for(unsigned i=0; i<r.size; i++) r.data[i] = 0.0f;

    for(unsigned i=0; i<l.size; i++){
        for(unsigned j=0; j<r.size; j++){
            r[j] += l[i] * matrix[i][j];
        }
    }
}

void Matrix::pull(){

    if(nopull) return;

    for(unsigned i=0; i<l.size; i++){
        float feedback = 0;
        for(unsigned j=0; j<r.size; j++){
            feedback += r[j] * matrix[i][j];
            changes[i][j] += l[i] * r[j];
        }
        l[i] = feedback;
    }
}

void Matrix::change(double factor){
    
    for(unsigned i=0; i<l.size; i++){
        for(unsigned j=0; j<r.size; j++){
            matrix[i][j] += changes[i][j] * (float)factor;
            changes[i][j] = 0.0f;
        }
    }
}

void Matrix::save(ui::Saver &saver){
    for(auto &i : matrix) for(auto &j : i) saver.write_float(j);
}

void Matrix::load(ui::Loader &loader){
    for(auto &i : matrix) for(auto &j : i) j = loader.read_float();
}

bool Matrix::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1
        && in[0].size > 0 && out[0].size > 0;
}

namespace Factory { extern std::string matrix; }
std::string Matrix::get_type(){ return Factory::matrix; };

// cmatrix ////////////////////////////////////////////////////////////////////

CMatrix::CMatrix(arrays in, arrays out, args a) :
    Layer(in, out, a), l(left[0]), r(right[0]),
    matrix(l.csize, std::vector<std::complex<float> >(r.csize)),
    changes(l.csize, std::vector<std::complex<float> >(r.csize, 0.0f))
{
    for(auto &i : matrix){
        for(auto &j : i){
            do { 
                j = {random_float(), random_float()};
            } while(std::abs(j) > 1.0f);
        }
    }
}

void CMatrix::push(){
    
    for(unsigned i=0; i<r.size; i++) r[i] = 0.0f;

    for(unsigned i=0; i<l.csize; i++){
        for(unsigned j=0; j<r.csize; j++){
            r(j) += l(i) * matrix[i][j];
        }
    }
}

void CMatrix::pull(){
   
    if(nopull) return;

    for(unsigned i=0; i<l.csize; i++){
        std::complex<float> feedback = 0.0f;
        for(unsigned j=0; j<r.csize; j++){
            feedback += r(j) * std::conj(matrix[i][j]);
            changes[i][j] += r(j) * std::conj(l(i));
        }
        l(i) = feedback;
    }
}

void CMatrix::change(double factor){

    for(unsigned i=0; i<l.csize; i++){
        for(unsigned j=0; j<r.csize; j++){
            matrix[i][j] += changes[i][j] * (float)factor;
            changes[i][j] = 0.0f;
        }
    }
}

void CMatrix::save(ui::Saver &saver){
    for(auto &i : matrix) for(auto &j : i) saver.write_complex(j);
}

void CMatrix::load(ui::Loader &loader){
    for(auto &i : matrix) for(auto &j : i) j = loader.read_complex();
}

bool CMatrix::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 &&
        in[0].size % 2 == 0 && out[0].size % 2 == 0 &&
        in[0].size > 0 && out[0].size > 0;
}

namespace Factory { extern std::string cmatrix; }
std::string CMatrix::get_type(){ return Factory::cmatrix; };

}

