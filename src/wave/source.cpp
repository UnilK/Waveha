#include "wave/source.h"

namespace wave{

Source::Source(){}

Source::~Source(){}

void Source::seek(uint32_t sample){}

std::vector<float> Source::pull(uint32_t amount){
    return std::vector<float>(amount, 0.0f);
}

std::vector<float> Source::get(uint32_t amount, uint32_t begin){
    return std::vector<float>(amount, 0.0f);
}

}
