#include "wave/util.h"

#include <algorithm>
#include <math.h>
#include <cstdint>

namespace wave {

sf::Int16 float_to_int(float num){
    return (sf::Int16)std::round(std::min((float)(1<<15)-1, std::max(-(float)(1<<15), num*(1<<15))));
}

float int_to_float(sf::Int16 num){
    return (float)num/(1<<15);
}

std::vector<sf::Int16> float_to_int(const std::vector<float> &data){
    return float_to_int(data.data(), data.size());
}

std::vector<float> int_to_float(const std::vector<sf::Int16> &data){
    return int_to_float(data.data(), data.size());
}

std::vector<sf::Int16> float_to_int(const float *data, unsigned size){
    std::vector<sf::Int16> conv(size);
    for(uint32_t i=0; i<size; i++) conv[i] = float_to_int(data[i]);
    return conv;
}

std::vector<float> int_to_float(const sf::Int16 *data, unsigned size){
    std::vector<float> conv(size);
    for(uint32_t i=0; i<size; i++) conv[i] = int_to_float(data[i]);
    return conv;
}

}

