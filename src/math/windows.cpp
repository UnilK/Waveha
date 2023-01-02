#include "math/windows.h"
#include "math/constants.h"

#include <cmath>

namespace math {

DCOSPrecalc dcosPrecalc;

DCOSPrecalc::DCOSPrecalc(){
   
    cosw.resize(N);

    double isq6 = 1.0 / std::sqrt(6);
    for(unsigned i=0; i<N; i++) cosw[i] = (1.0 - std::cos(2*PI*i/N)) * isq6;
}

float dcos(float x){ return 0.0f; }

}

