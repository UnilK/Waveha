#include "ml/util.h"

namespace ml {

float random_float(){ return (float)rand() / RAND_MAX; }

float sign(float x){ return (float)((x>0.0)-(x<0.0)); }

}

