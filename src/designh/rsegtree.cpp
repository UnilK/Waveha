#include "designh/rsegtree.h"

#include <cstring>

namespace designh {

rsegtree::rsegtree(){}

rsegtree::rsegtree(int size){
    init(size);
}

void rsegtree::init(int size){

    bsize = size;
    wsize = 4 * bsize;
    
    ssize = 1;
    while(ssize < wsize) ssize *= 2;

    pointer = 0;
    offset = 0;
    
    segtree.resize(2 * ssize, -1e9);
}

void rsegtree::push(float energy){
    
    if(pointer + bsize >= wsize){
        
        std::memcpy(
                segtree.data() + ssize,
                segtree.data() + ssize + pointer,
                sizeof(float) * bsize);
        pointer = 0;
    
        int l = ssize / 2;
        int r = (ssize + bsize - 1) / 2;

        while(l){
            for(int i=l; i<=r; i++) segtree[i] = std::max(segtree[2*i], segtree[2*i+1]);
            l /= 2; r /= 2;
        }
    }

    int i = ssize + bsize + pointer;
    segtree[i] = energy;

    for(i /= 2; i > 0; i /= 2) segtree[i] = std::max(segtree[2*i], segtree[2*i+1]);
    pointer++;
}

float rsegtree::max(int left, int right){
    
    left += pointer + offset + ssize;
    right += pointer + offset + ssize;

    float top = -1e9;
    
    while(left <= right){
        if(left&1) top = std::max(top, segtree[left++]);
        if(~right&1) top = std::max(top, segtree[right--]);
        left /= 2; right /= 2;
    }

    return top;
}

float& rsegtree::operator[](int index){
    return segtree[ssize + pointer + offset + index];
}

void rsegtree::set_offset(int o){ offset = o; }

int rsegtree::size() const { return bsize; }
int rsegtree::left() const { return offset; }
int rsegtree::right() const { return bsize - offset; }

}

