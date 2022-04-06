#include "ml/field.h"
#include "ml/util.h"

#include <cmath>

namespace ml {

// v1 /////////////////////////////////////////////////////////////////////////

void v1::f(float *in, float *out){
    
    float x = std::abs(in[0]);
    float s = sign(in[0]);

    float y;

    if(x <= 1.0f){
        y = x*x / 3.0f;
    } else {
        y = (1.5f - 1.0f / x) / 1.5f;
    }
    
    out[0] = s * y;
}

void v1::df(float *in, float *out){
    
    float x = std::abs(out[0]);
    float dx = in[0];

    float y;

    if(x <= 1.0f){
        y = x / 1.5f;
    } else {
        y = 1.0f / (x*x) / 1.5f;
    }

    out[0] = dx * y;
}

// s1 /////////////////////////////////////////////////////////////////////////

void s1::f(float *in, float *out){
    
    float x = std::abs(in[0]);
    float s = sign(in[0]);

    float y;

    if(x <= 1.0f){
        y = x*x*x / 3.0f;
    } else {
        y = std::log(x) + 1.0f / 3.0f;
    }
    
    out[0] = s * y;
}

void s1::df(float *in, float *out){
    
    float x = std::abs(out[0]);
    float dx = in[0];

    float y;

    if(x <= 1.0f){
        y = x*x;
    } else {
        y = 1.0f / x / 3.0f;
    }

    out[0] = dx * y;
}

// v2 /////////////////////////////////////////////////////////////////////////

void v2::f(float *in, float *out){

    float d = std::sqrt(in[0]*in[0] + in[1]*in[1]);
    
    float nr = in[0];
    float ni = in[1];
    if(d != 0.0f){ nr /= d; ni /= d; }

    float y;
    
    if(d <= 1.0f){
        y = d*d / 3.0f;
    } else {
        y = (1.5f - 1.0f / d) / 1.5f;
    }

    out[0] = y * nr;
    out[1] = y * ni;
}

void v2::df(float *in, float *out){

    float d = std::sqrt(out[0]*out[0] + out[1]*out[1]);
    
    float nr = out[0];
    float ni = out[1];
    if(d != 0.0f){ nr /= d; ni /= d; }
    
    float y;
    
    if(d <= 1.0f){
        y = d / 1.5f;
    } else {
        y = 1.0f / (d*d) / 1.5f;
    }

    float dd = std::sqrt(in[0]*in[0] + in[1]*in[1]);
    
    float dr = in[0];
    float di = in[1];
   
    float dot = std::abs(nr * dr + ni * di);
    
    if(dd != 0.0f){ dr /= dd; di /= dd; }

    out[0] = dr * y * dot;
    out[1] = di * y * dot;
}

// average1 ///////////////////////////////////////////////////////////////////

void average1::f(float *in, float *out){
    out[0] = in[0] * in[0];
}

void average1::df(float *in, float *out){
    out[0] = 2.0 * in[0];
}

// average2 ///////////////////////////////////////////////////////////////////

void average2::f(float *in, float *out){
    float d = std::sqrt(in[0]*in[0] + in[1]*in[1]);
    out[0] = d*d;
    out[1] = 0.0f;
}

void average2::df(float *in, float *out){
    out[0] = 2.0 * in[0];
    out[1] = 2.0 * in[1];
}

// abs1 ///////////////////////////////////////////////////////////////////////

void abs1::f(float *in, float *out){
    out[0] = in[0] * sign(in[0]);
}

void abs1::df(float *in, float *out){
    out[0] = in[0] * sign(out[0]);
}

// abs2 ///////////////////////////////////////////////////////////////////////

void abs2::f(float *in, float *out){
    out[0] = std::sqrt(in[0]*in[0] + in[1]*in[1]);
}

void abs2::df(float *in, float *out){
    
    float d = std::sqrt(out[0]*out[0] + out[1]*out[1]);
    
    if(d == 0.0f) d = 1.0f;

    out[0] = in[0] * out[0] / d;
    out[1] = in[0] * out[1] / d;
}

// up2 ////////////////////////////////////////////////////////////////////////

void up2::f(float *in, float *out){
    out[0] = in[0];
    out[1] = 0.0f;
}

void up2::df(float *in, float *out){
    out[0] = in[0];
}

// boilerplate ////////////////////////////////////////////////////////////////

namespace Factory { extern std::string v1; }
std::string V1Field::get_type(){ return Factory::v1; };

namespace Factory { extern std::string s1; }
std::string S1Field::get_type(){ return Factory::s1; };

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

}

