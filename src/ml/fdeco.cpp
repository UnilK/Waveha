#include "ml/fdeco.h"

#include <cmath>
#include <iostream>

namespace ml {

void Fdeco::push(){
    
    if(left.size() == 0) return;
    
    for(unsigned i=0; i<cleft.size(); i++) cleft[i] = {left[2*i], left[2*i+1]};

    /*
    phase = {1.0f, 0.0f};
    if(std::abs(cleft[0]) != 0.0f) phase = cleft[0] / std::abs(cleft[0]);
    
    std::complex<float> sum = std::conj(phase);
    for(unsigned i=1; i<cleft.size(); i++){
        cleft[i] *= sum;
        sum *= std::conj(phase);
    }
    
    */

    phase = {0.0f, 0.0f};
    if(std::abs(cleft[0]) != 0.0f) phase = cleft[0] / std::abs(cleft[0]);

    for(unsigned i=1; i<cleft.size(); i++){
         phase += cleft[i] * std::conj(cleft[i-1]);
    }
    
    double a = -std::arg(phase);

    for(unsigned i=1; i<cleft.size(); i++) cleft[i] *= std::polar(1.0, a*i);

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

void Fdeco::pull(){
    
    for(unsigned i=0; i<right.size(); i+=2) cright[i/2] = {right[i], right[i+1]};
    
    for(unsigned i=0; i<cleft.size(); i++){
        std::complex<float> feedback = 0.0f;
        for(unsigned j=0; j<cright.size(); j++){
            feedback += cright[j] * matrix[i][j];
            changes[i][j] += cleft[i] * cright[j];
        }
        cleft[i] = feedback;
    }
    
    std::complex<float> sum = phase;
    for(unsigned i=1; i<cleft.size(); i++){
        cleft[i] *= sum;
        sum *= phase;
    }

    for(unsigned i=0; i<cleft.size(); i++){
        left[2*i] = cleft[i].real();
        left[2*i+1] = cleft[i].imag();
    }

}

namespace Factory { extern std::string fdeco; }
std::string Fdeco::get_type(){ return Factory::fdeco; }

}

