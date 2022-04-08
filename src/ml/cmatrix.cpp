#include "ml/cmatrix.h"
#include "ml/util.h"

#include <assert.h>

namespace ml {

CMatrix::CMatrix(
        std::vector<float> &source,
        std::vector<float> &destination) :
    Layer(source, destination),
    cleft(left.size() / 2),
    cright(right.size() / 2),
    matrix(left.size() / 2, std::vector<std::complex<float> >(right.size() / 2)),
    changes(left.size() / 2, std::vector<std::complex<float> >(right.size() / 2, 0.0f))
{
    for(auto &i : matrix) for(auto &j : i) j = random_float();
}

void CMatrix::push(){
    
    for(unsigned i=0; i<left.size(); i+=2) cleft[i/2] = {left[i], left[i+1]};
    for(auto &i : cright) i = 0.0f;

    for(unsigned i=0; i<cleft.size(); i++){
        for(unsigned j=0; j<cright.size(); j++){
            cright[j] += cleft[i] * matrix[i][j];
        }
    }

    for(unsigned i=0; i<cright.size(); i++){
        right[2*i] = cright[i].real();
        right[2*i+1] = cright[i].imag();
    }
}

void CMatrix::pull(){
   
    for(unsigned i=0; i<right.size(); i+=2) cright[i/2] = {right[i], right[i+1]};
    
    for(unsigned i=0; i<cleft.size(); i++){
        std::complex<float> feedback = 0.0f;
        for(unsigned j=0; j<cright.size(); j++){
            feedback += cright[j] * matrix[i][j];
            changes[i][j] += cleft[i] * cright[j];
        }
        cleft[i] = feedback;
    }
    
    for(unsigned i=0; i<cleft.size(); i++){
        left[2*i] = cleft[i].real();
        left[2*i+1] = cleft[i].imag();
    }
}

void CMatrix::change(double factor){

    for(unsigned i=0; i<cleft.size(); i++){
        for(unsigned j=0; j<cright.size(); j++){
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

bool CMatrix::ok(std::vector<float> &left, std::vector<float> &right){
    return (left.size() % 2 == 0) && (right.size() % 2 == 0);
}

namespace Factory { extern std::string cmatrix; }
std::string CMatrix::get_type(){ return Factory::cmatrix; };

}

