#include "wave/source.h"

namespace wave{

Source::Source(){}

Source::~Source(){}

unsigned Source::pull_loop(unsigned amount, std::vector<float> &samples){

    samples = std::vector<float>(amount, 0.0f);

    if(!good || size() == 0) return 0;

    unsigned position = 0, left = amount;

    while(left){
        
        unsigned jump = std::min(left, size() - tell() % size());

        auto segment = get(jump, tell());

        for(unsigned i=0; i<jump; i++) samples[position++] = segment[i];
        
        seek((tell() + jump) % size());

        left -= jump;
    }

    return amount;
}

std::vector<float> Source::get_loop(unsigned amount, unsigned begin){

    std::vector<float> samples(amount, 0.0f);

    if(!good || size() == 0) return samples;

    unsigned position = 0, left = amount;

    while(left){
        
        unsigned jump = std::min(left, size() - begin % size());

        auto segment = get(jump, begin);

        for(unsigned i=0; i<jump; i++) samples[position++] = segment[i];
        
        begin += jump;
        begin %= size();

        left -= jump;
    }

    return samples;
}

}
