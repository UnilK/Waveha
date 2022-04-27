#pragma once

#include "ml/layer.h"

#include <vector>
#include <array>

namespace ml {

template<unsigned N, unsigned M, unsigned V, typename F>
class Field : public Layer {

    /*
       N-to-M-dimensional field compression with function F::f and gradient F::df that
       uses V variables. f and df return a void and take three and four float
       pointers as arguments respectively: left, right, var; left, right, var, change.
    */

public:

    Field(
            std::vector<float> &source,
            std::vector<float> &destination,
            std::array<float, V> initial = {});

    void push();
    void pull();
    void change(double factor);
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    static bool ok(std::vector<float> &left, std::vector<float> &right);

protected:

    std::vector<std::array<float, V> > variables, changes;

};

#include "ml/field.inl"

struct a1 { // acceleration analog in 1d
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct v1 { // velocity analog in 1d
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct vv1 { // velocity analog in 1d with variables
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct s1 { // distance analog in 1d
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct e11 { // empirically created compressor
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};


struct v2 { // velocity analog in complex plane
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct abs1 { // absolute value, distance from origo
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct abs2 { // 2D absolute value
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct up2 { // scale 1D to 2D
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
};

struct relu { // max(0, x)
    static void f(float *left, float *right, float *var);
    static void df(float *left, float *right, float *var, float *change);
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

class VV1Field : public Field<1, 1, 2, vv1> {
public:
    VV1Field(std::vector<float> &source, std::vector<float> &destination);
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

