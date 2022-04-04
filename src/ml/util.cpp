#include "ml/util.h"

namespace ml {

float random_float(){ return 2.0f * (float)rand() / RAND_MAX - 1.0f; }

float sign(float x){ return (float)((x>0.0)-(x<0.0)); }

}

