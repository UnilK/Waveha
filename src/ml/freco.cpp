#include "ml/freco.h"

namespace ml {

void Freco::push(){
    
    for(unsigned i=0; i<left.size(); i+=2) cleft[i/2] = {left[i], left[i+1]};
    for(auto &i : cright) i = 0.0f;

    for(unsigned i=0; i<cleft.size(); i++){
        if(std::abs(cleft[i]) == 0.0f) continue;
        std::complex<float> norm = cleft[i] / std::abs(cleft[i]), sum = cleft[i];
        for(unsigned j=0; j<cright.size(); j++){
            cright[j] += matrix[i][j] * sum;
            sum *= norm;
        }
    }

    for(unsigned i=0; i<cright.size(); i++){
        right[2*i] = cright[i].real();
        right[2*i+1] = cright[i].imag();
    }
}

void Freco::pull(){

    for(unsigned i=0; i<right.size(); i+=2) cright[i/2] = {right[i], right[i+1]};
    
    for(unsigned i=0; i<cleft.size(); i++){
       
        if(std::abs(cleft[i]) == 0.0f) continue;

        std::complex<float> feedback = 0.0f, norm = cleft[i] / std::abs(cleft[i]);
        std::complex<float> perp = {norm.imag(), -norm.real()};
        std::complex<float> sum = cleft[i], sumd = perp;
        
        for(unsigned j=0; j<cright.size(); j++){
            float dr = cright[j].real() * perp.real() + cright[j].imag() * perp.imag();
            float da = cright[j].real() * norm.real() + cright[j].imag() * norm.imag();
            feedback += ((float)(j+1) * sumd * dr + norm * da) * matrix[i][j];
            changes[i][j] += sum * cright[j];
            sum *= norm;
            sumd *= norm;
        }

        cleft[i] = feedback;
    }
    
    for(unsigned i=0; i<cleft.size(); i++){
        left[2*i] = cleft[i].real();
        left[2*i+1] = cleft[i].imag();
    }
}

namespace Factory { extern std::string freco; }
std::string Freco::get_type(){ return Factory::freco; }

}

