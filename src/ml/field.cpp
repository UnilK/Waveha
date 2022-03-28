#include "ml/field.h"
#include "ml/util.h"

#include <cmath>

namespace ml {

void v1::f(float *in, float *out){
    
    float x = std::abs(in[0]);
    float s = sign(in[0]);
    
    if(x <= 1.0f){
        out[0] = s * 0.5f * x*x / 1.5f;
    } else {
        out[0] = s * (1.5f - 1.0f / x) / 1.5f;
    }
}

void v1::g(float *in, float *out){
    
    float x = std::abs(out[0]);
    float s = sign(out[0]);
    float dx = in[0];
    
    if(x <= 1.0f){
        out[0] = s * dx * x / 1.5f;
    } else {
        out[0] = s * dx / (x*x) / 1.5f;
    }
}



// f(c) = abs(c)*c, abs(c) <= 1
// otherwise f(c) = polar(arg(c)) * (1.5 - 1 / abs(c))
void v2::f(float *in, float *out){

    float r = in[0];
    float i = in[1];
    float d = std::sqrt(r*r + i*i);
    
    if(d <= 1.0f){
        out[0] = 0.5f * r * d / 1.5f;
        out[1] = 0.5f * i * d / 1.5f;
    } else {
        float norm = (1.5f - 1.0f / d) / d / 1.5f;
        out[0] = r * norm;
        out[1] = i * norm;
    }
}

void v2::g(float *in, float *out){

    float r = out[0];
    float i = out[1];
    float dr = in[0];
    float di = in[1];
    float d = std::sqrt(r*r + i*i);
    float dot = r * dr + i * di;
    
    if(d <= 1.0f){
        out[0] = dr * dot / 1.5f;
        out[1] = di * dot / 1.5f;
    } else {
        float norm = (1.5f - 1.0f / (d*d)) / d / 1.5f;
        out[0] = dr * norm * dot;
        out[1] = di * norm * dot;
    }
}



void average1::f(float *in, float *out){
    out[0] = in[0] * in[0];
}

void average1::g(float *in, float *out){
    out[0] = 2 * out[0] * in[0];
}



void average2::f(float *in, float *out){
    
    float r = in[0];
    float i = in[1];
    float d = std::sqrt(r*r + i*i);
    out[0] = r * d;
    out[1] = i * d;
}

void average2::g(float *in, float *out){
    
    float r = out[0];
    float i = out[1];
    float dr = in[0];
    float di = in[1];
    float dot = r * dr + i * di;

    out[0] = 2 * dot * dr;
    out[1] = 2 * dot * di;
}



namespace Factory { extern std::string v1; }
std::string V1Field::get_type(){ return Factory::v1; };

namespace Factory { extern std::string v2; }
std::string V2Field::get_type(){ return Factory::v2; };

namespace Factory { extern std::string average1; }
std::string Average1Field::get_type(){ return Factory::average1; };

namespace Factory { extern std::string average2; }
std::string Average2Field::get_type(){ return Factory::average2; };

}

