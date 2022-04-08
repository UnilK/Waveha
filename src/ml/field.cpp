#include "ml/field.h"
#include "ml/util.h"

#include <cmath>

namespace ml {

// a1 /////////////////////////////////////////////////////////////////////////

void a1::f(float *left, float *right, float *var){
    
    float x = std::abs(left[0]);
    float s = sign(left[0]);

    float y;

    if(x <= 1.0f){
        y = x / 1.5f;
    } else {
        y = (1.5f - 0.5f / (x*x)) / 1.5f;
    }
    
    right[0] = s * y;
}

void a1::df(float *left, float *right, float *var, float *change){
    
    float x = std::abs(left[0]);
    float dx = right[0];

    float y;

    if(x <= 1.0f){
        y = 1.0f / 1.5f;
    } else {
        y = 1.0f / 1.5f / (x*x*x);
    }

    left[0] = dx * y;
}

// v1 /////////////////////////////////////////////////////////////////////////

void v1::f(float *left, float *right, float *var){
    
    float x = std::abs(left[0]);
    float s = sign(left[0]);

    float y;

    if(x <= 1.0f){
        y = x*x / 3.0f;
    } else {
        y = (1.5f - 1.0f / x) / 1.5f;
    }
    
    right[0] = s * y;
}

void v1::df(float *left, float *right, float *var, float *change){
    
    float x = std::abs(left[0]);
    float dx = right[0];

    float y;

    if(x <= 1.0f){
        y = x / 1.5f;
    } else {
        y = 1.0f / (x*x) / 1.5f;
    }

    left[0] = dx * y;
}

// vv1 ////////////////////////////////////////////////////////////////////////

void vv1::f(float *left, float *right, float *var){
    
    float x = std::abs(left[0] * var[0] + var[1]);
    float s = sign(left[0]);

    float y;

    if(x <= 1.0f){
        y = x*x / 3.0f;
    } else {
        y = (1.5f - 1.0f / x) / 1.5f;
    }
    
    right[0] = s * y;
}

void vv1::df(float *left, float *right, float *var, float *change){
    
    float x = std::abs(left[0] * var[0] + var[1]);
    float dx = right[0];

    float y;

    if(x <= 1.0f){
        y = x / 1.5f;
    } else {
        y = 1.0f / (x*x) / 1.5f;
    }

    change[0] += left[0] * dx * y * 0.01;
    change[1] += dx * y;
    left[0] = var[0] * dx * y;
}

// s1 /////////////////////////////////////////////////////////////////////////

void s1::f(float *left, float *right, float *var){
    
    float x = std::abs(left[0]);
    float s = sign(left[0]);

    float y;

    if(x <= 1.0f){
        y = x*x*x / 3.0f;
    } else {
        y = std::log(x) + 1.0f / 3.0f;
    }
    
    right[0] = s * y;
}

void s1::df(float *left, float *right, float *var, float *change){
    
    float x = std::abs(left[0]);
    float dx = right[0];

    float y;

    if(x <= 1.0f){
        y = x*x;
    } else {
        y = 1.0f / x;
    }

    left[0] = dx * y;
}

// e11 ////////////////////////////////////////////////////////////////////////

void e11::f(float *left, float *right, float *var){
    
    float x = std::abs(left[0]);
    float s = sign(left[0]);

    float y;

    if(x <= 1.0f){
        y = x / 1.5f;
    } else {
        y = (1.5f - 0.5f / (x*x)) / 1.5f;
    }
    
    right[0] = s * y;
}

void e11::df(float *left, float *right, float *var, float *change){
    
    float x = std::abs(left[0]);
    float dx = right[0];

    float y;

    if(x <= 1.0f){
        y = 1.0f / 1.5f;
    } else {
        y = 1.0f / 1.5f / (x*x*x);
    }

    left[0] = dx * y;
}

// v2 /////////////////////////////////////////////////////////////////////////

void v2::f(float *left, float *right, float *var){

    float d = std::sqrt(left[0]*left[0] + left[1]*left[1]);
    
    float nr = left[0];
    float ni = left[1];
    if(d != 0.0f){ nr /= d; ni /= d; }

    float y;
    
    if(d <= 1.0f){
        y = d*d / 3.0f;
    } else {
        y = (1.5f - 1.0f / d) / 1.5f;
    }

    right[0] = y * nr;
    right[1] = y * ni;
}

void v2::df(float *left, float *right, float *var, float *change){

    float d = std::sqrt(left[0]*left[0] + left[1]*left[1]);
    
    float nr = left[0];
    float ni = left[1];
    if(d != 0.0f){ nr /= d; ni /= d; }
    
    float y;
    
    if(d <= 1.0f){
        y = d / 1.5f;
    } else {
        y = 1.0f / (d*d) / 1.5f;
    }

    float dd = std::sqrt(right[0]*right[0] + right[1]*right[1]);
    
    float dr = right[0];
    float di = right[1];
   
    float dot = std::abs(nr * dr + ni * di);
    
    if(dd != 0.0f){ dr /= dd; di /= dd; }

    left[0] = dr * y * dot;
    left[1] = di * y * dot;
}

// average1 ///////////////////////////////////////////////////////////////////

void average1::f(float *left, float *right, float *var){
    right[0] = left[0] * left[0];
}

void average1::df(float *left, float *right, float *var, float *change){
    left[0] = 2.0 * right[0];
}

// average2 ///////////////////////////////////////////////////////////////////

void average2::f(float *left, float *right, float *var){
    float d = std::sqrt(left[0]*left[0] + left[1]*left[1]);
    right[0] = d*d;
    right[1] = 0.0f;
}

void average2::df(float *left, float *right, float *var, float *change){
    left[0] = 2.0 * right[0];
    left[1] = 2.0 * right[1];
}

// abs1 ///////////////////////////////////////////////////////////////////////

void abs1::f(float *left, float *right, float *var){
    right[0] = left[0] * sign(left[0]);
}

void abs1::df(float *left, float *right, float *var, float *change){
    left[0] = right[0] * sign(left[0]);
}

// abs2 ///////////////////////////////////////////////////////////////////////

void abs2::f(float *left, float *right, float *var){
    right[0] = std::sqrt(left[0]*left[0] + left[1]*left[1]);
}

void abs2::df(float *left, float *right, float *var, float *change){
    
    float d = std::sqrt(left[0]*left[0] + left[1]*left[1]);
    
    if(d == 0.0f) d = 1.0f;

    left[0] = right[0] * left[0] / d;
    left[1] = right[0] * left[1] / d;
}

// up2 ////////////////////////////////////////////////////////////////////////

void up2::f(float *left, float *right, float *var){
    right[0] = left[0];
    right[1] = 0.0f;
}

void up2::df(float *left, float *right, float *var, float *change){
    left[0] = right[0];
}

// relu ///////////////////////////////////////////////////////////////////////

void relu::f(float *left, float *right, float *var){
    right[0] = std::max(0.0f, left[0]);
}

void relu::df(float *left, float *right, float *var, float *change){
    left[0] = right[0] * (left[0] >= 0);
}

// boilerplate ////////////////////////////////////////////////////////////////

namespace Factory { extern std::string a1; }
std::string A1Field::get_type(){ return Factory::a1; };

namespace Factory { extern std::string v1; }
std::string V1Field::get_type(){ return Factory::v1; };

VV1Field::VV1Field(std::vector<float> &source, std::vector<float> &destination) :
    Field(source, destination, {1, 0}) {}
namespace Factory { extern std::string vv1; }
std::string VV1Field::get_type(){ return Factory::vv1; };

namespace Factory { extern std::string s1; }
std::string S1Field::get_type(){ return Factory::s1; };

namespace Factory { extern std::string e11; }
std::string E11Field::get_type(){ return Factory::e11; };

namespace Factory { extern std::string v2; }
std::string V2Field::get_type(){ return Factory::v2; };

namespace Factory { extern std::string average1; }
std::string Average1Field::get_type(){ return Factory::average1; };

namespace Factory { extern std::string average2; }
std::string Average2Field::get_type(){ return Factory::average2; };

namespace Factory { extern std::string abs1; }
std::string Abs1Field::get_type(){ return Factory::abs1; };

namespace Factory { extern std::string abs2; }
std::string Abs2Field::get_type(){ return Factory::abs2; };

namespace Factory { extern std::string up2; }
std::string Up2Field::get_type(){ return Factory::up2; };

namespace Factory { extern std::string relu; }
std::string ReluField::get_type(){ return Factory::relu; };

}

