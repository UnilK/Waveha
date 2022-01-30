#pragma once

#include <vector>

#include <SFML/Config.hpp>

namespace wave {

sf::Int16 float_to_int(float num);
float int_to_float(sf::Int16 num);

std::vector<sf::Int16> float_to_int(const std::vector<float> &data);
std::vector<float> int_to_float(const std::vector<sf::Int16> &data);

std::vector<sf::Int16> float_to_int(const float *data, unsigned size);
std::vector<float> int_to_float(const sf::Int16 *data, unsigned size);

}
