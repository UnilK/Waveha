#pragma once

#include "ml/layer.h"

#include <assert.h>

namespace ml {

template<unsigned N, typename F> class Field : public Layer {

    /*
       N-dimensional field compression with function f and gradient g
       f and g return a void and take two float pointers as arguments:
       in and out.
    */

public:

    Field(std::vector<float> &source, std::vector<float> &destination) : 
        Layer(source, destination)
    {
        assert(left.size() == right.size() && left.size()%N == 0);
    }

    void push(){
        for(unsigned i=0; i<left.size(); i+=N) F::f(left.data()+i, right.data()+i);
    }
    
    void pull(){
        for(unsigned i=0; i<left.size(); i+=N) F::g(right.data()+i, left.data()+i);
    }
};

struct v1 { // velocity analog in 1d
    static void f(float *in, float *out);
    static void g(float *in, float *out);
};

struct v2 { // velocity analog in complex plane
    static void f(float *in, float *out);
    static void g(float *in, float *out);
};

struct average1 { // average analog for 1d
    static void f(float *in, float *out);
    static void g(float *in, float *out);
};

struct average2 { // average analog for complex plane
    static void f(float *in, float *out);
    static void g(float *in, float *out);
};

class V1Field : public Field<1, v1> {
    using Field::Field;
    std::string get_type();
};

class V2Field : public Field<2, v1> {
    using Field::Field;
    std::string get_type();
};

class Average1Field : public Field<1, average1> {
    using Field::Field;
    std::string get_type();
};

class Average2Field : public Field<2, average2> {
    using Field::Field;
    std::string get_type();
};

}

