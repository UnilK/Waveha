#pragma once

#include "ml/layer.h"

#include <assert.h>

namespace ml {

template<unsigned N, unsigned M, typename F> class Field : public Layer {

    /*
       N-to-M-dimensional field compression with function f and gradient df.
       f and df return a void and take two float pointers as arguments:
       in and out.
    */

public:

    Field(std::vector<float> &source, std::vector<float> &destination) : 
        Layer(source, destination)
    {}

    void push(){
        for(unsigned i=0, j=0; i<left.size(); i+=N, j+=M) F::f(left.data()+i, right.data()+j);
    }
    
    void pull(){
        for(unsigned i=0, j=0; i<left.size(); i+=N, j+=M) F::df(right.data()+j, left.data()+i);
    }

    static bool ok(std::vector<float> &left, std::vector<float> &right){
        return left.size() * M == right.size() * N;
    }
};

struct v1 { // velocity analog in 1d
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct s1 { // position analog in 1d
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct v2 { // velocity analog in complex plane
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct average1 { // average analog for 1d
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct average2 { // average analog for complex plane
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct abs1 { // absolute value, distance from origo
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct abs2 { // 2D absolute value
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct up2 { // scale 1D to 2D
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

class V1Field : public Field<1, 1, v1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class S1Field : public Field<1, 1, s1> {
    using Field::Field;
    std::string get_type();
};

class V2Field : public Field<2, 2, v2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Average1Field : public Field<1, 1, average1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Average2Field : public Field<2, 2, average2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Abs1Field : public Field<1, 1, abs1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Abs2Field : public Field<2, 1, abs2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Up2Field : public Field<1, 2, up2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

}

