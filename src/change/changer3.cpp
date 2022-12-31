#pragma GCC target("avx2")
#pragma GCC optimize("O3")
#include "change/changer3.h"
#include "math/constants.h"
#include "math/fft.h"

#include <cassert>
#include <algorithm>
#include <cmath>

#include <iostream>
#include <iomanip>

namespace change {

Changer3::Changer3(int rate){}

float Changer3::process(float sample){ return sample; }

std::vector<float> Changer3::debug(){ return {}; }

}

