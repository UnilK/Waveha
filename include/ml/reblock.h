#pragma once

#include "ml/layer.h"

namespace ml {

class Reblock : public Layer {

    // chages vector blocking. for example:
    // .... .. ...... => .. ..........

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};



class Interleave : public Layer {

    /*
       Interleaves vectors:
       3 4 8 2 4
       ....
         ....
           ....
       ->
       ........
       ->
       ....
           ....

       takes 5 numbers as arguments :
       input vector amount and size
       intermediate vector size
       output vector amount and size
    */

public:

    Interleave(arrays in, arrays out, args);

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

private:

    std::vector<float> middle;
    std::vector<unsigned> lpos, rpos, lcnt, rcnt;

};



class Copy : public Layer {

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};



class Join : public Layer {

public:

    using Layer::Layer;

    void push();
    void pull();

    std::string get_type();
    static bool ok(arrays in, arrays out, args a);

};

}

