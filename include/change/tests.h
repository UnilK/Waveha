#pragma once

#include <vector>

namespace change {

std::vector<float> translate(const std::vector<float> &audio);

std::vector<float> pitchenvelope(const std::vector<float> &audio);

std::vector<float> frequencywindow();

std::vector<float> energytranslate(const std::vector<float> &audio);

std::vector<float> energytranslate2(const std::vector<float> &audio);

std::vector<float> hodgefilter(const std::vector<float> &audio);

}

