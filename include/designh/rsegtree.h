#pragma once

#include <vector>

namespace designh {

class rsegtree {

public:

    rsegtree();
    rsegtree(int size);
    void init(int size);
    
    void push(float energy);
    float max(int left, int right);

    float& operator[](int index);
    void set_offset(int);

    int size() const;
    int left() const;
    int right() const;

private:

    int bsize, wsize, ssize, pointer, offset;
    std::vector<float> segtree;
};

}

