#include "designf/peakfinder.h"

#include <algorithm>
#include <cassert>

namespace designf {

PeakFinder::PeakFinder(int windowSize){

    assert("window size must be positive" && windowSize > 0);

    index = 0;
    size = 2 * windowSize;

    tsize = 1;
    while(tsize < size) tsize *= 2;

    segtree.resize(2 * tsize, -1e9f);
}

bool PeakFinder::process(float sample){
    
    index = (index + 1) % size;

    int opposite = (index + size / 2) % size;

    segtree[tsize + opposite] = sample;

    for(int i=(tsize+opposite)/2; i>0; i/=2){
        segtree[i] = std::max(segtree[2*i], segtree[2*i+1]);
    }

    return segtree[tsize + index] == segtree[1];
}

int PeakFinder::get_delay(){
    return size / 2;
}


}

