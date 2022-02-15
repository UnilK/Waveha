#include "wave/loop.h"

#include <algorithm>

namespace wave {

Loop::Loop(Source *s){
    open(s);
}

Loop::~Loop(){}

void Loop::open(Source *s){
    source = s;
    channels = source->channels;
    frameRate = source->frameRate;
    seek(0);
}

void Loop::seek(unsigned sample){
    read = sample % source->size();
}

unsigned Loop::tell(){
    return read;
}

unsigned Loop::size(){
    return source->size();
}

unsigned Loop::pull(unsigned amount, std::vector<float> &samples){

    samples = std::vector<float>(amount, 0.0f);

    unsigned position = 0, left = amount;

    while(left){
        
        unsigned jump = std::min(left, size() - read % size());

        auto segment = source->get(jump, read);

        for(unsigned i=0; i<jump; i++) samples[position++] = segment[i];
        
        read += jump;
        read %= size();

        left -= jump;
    }

    return amount;
}

std::vector<float> Loop::get(unsigned amount, unsigned begin){

    std::vector<float> samples(amount, 0.0f);

    unsigned position = 0, left = amount;

    while(left){
        
        unsigned jump = std::min(left, size() - begin % size());

        auto segment = source->get(jump, begin);

        for(unsigned i=0; i<jump; i++) samples[position++] = segment[i];
        
        begin += jump;
        begin %= size();

        left -= jump;
    }

    return samples;
}

}

