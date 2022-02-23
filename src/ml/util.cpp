#include "ml/util.h"

namespace ml {

std::complex<float> random_complex(){
    return {(float)rand() / RAND_MAX, (float)rand() / RAND_MAX };
}

}

