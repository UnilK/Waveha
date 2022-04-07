#pragma once

#include "ml/layer.h"

#include <vector>
#include <array>

namespace ml {

template<unsigned N, unsigned M, unsigned V, typename F>
class Field : public Layer {

    /*
       N-to-M-dimensional field compression with function f and gradient df.
       f and df return a void and take two float pointers as arguments:
       in and out. The compression uses V variables
    */

public:

    Field(std::vector<float> &source, std::vector<float> &destination);

    void push();
    void pull();

    static bool ok(std::vector<float> &left, std::vector<float> &right);

protected:

    std::vector<std::array<float, V> > var;

};

#include "ml/field.inl"

struct a1 { // acceleration analog in 1d
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct v1 { // velocity analog in 1d
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct s1 { // distance analog in 1d
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct e11 { // empirically created compressor
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};


struct v2 { // velocity analog in complex plane
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct average1 { // average analog for 1d. use for evaluation
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

struct average2 { // average analog for complex plane. use for evaluation
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

struct relu { // max(0, x)
    static void f(float *in, float *out);
    static void df(float *in, float *out);
};

class A1Field : public Field<1, 1, 0, a1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class V1Field : public Field<1, 1, 0, v1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class S1Field : public Field<1, 1, 0, s1> {
    using Field::Field;
    std::string get_type();
};

class E11Field : public Field<1, 1, 0, e11> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class V2Field : public Field<2, 2, 0, v2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Average1Field : public Field<1, 1, 0, average1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Average2Field : public Field<2, 2, 0, average2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Abs1Field : public Field<1, 1, 0, abs1> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Abs2Field : public Field<2, 1, 0, abs2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class Up2Field : public Field<1, 2, 0, up2> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

class ReluField : public Field<1, 1, 0, relu> {
public:
    using Field::Field;
    using Field::ok;
    std::string get_type();
};

}

