#include "ml/multiply.h"

namespace ml {

// multiply ///////////////////////////////////////////////////////////////////

Multiply::Multiply(arrays in, arrays out, args a) :
    Layer(in, out, a)
{
    for(auto i : a) if(i == "static") fnopull = 1;
}

void Multiply::push(){
    
    for(unsigned i=0; i<left[0].size; i++){
        for(unsigned j=0; j<left[1].size; j++){
            right[0][i*left[1].size + j] = left[0][i] * left[1][j];
        }
    }
}

void Multiply::pull(){
    
    if(nopull) return;

    for(unsigned i=0; i<left[0].size; i++) left[0][i] = 0.0f;
    for(unsigned i=0; i<left[1].size; i++) left[1][i] = 0.0f;

    if(fnopull){
        for(unsigned i=0; i<left[0].size; i++){
            for(unsigned j=0; j<left[1].size; j++){
                left[1][i] += left[0][j] * right[0][i*left[1].size + j];
            }
        }
    }
    else {
        for(unsigned i=0; i<left[0].size; i++){
            for(unsigned j=0; j<left[1].size; j++){
                left[1][i] += left[0][j] * right[0][i*left[1].size + j];
                left[0][i] += left[1][j] * right[0][i*left[1].size + j];
            }
        }
    }
}

bool Multiply::ok(arrays in, arrays out, args a){
    if(in.size() == 2 && out.size() == 1 && in[0].size * in[1].size == out[0].size) return 1;
    return 0;
}

namespace Factory { extern std::string multiply; }
std::string Multiply::get_type(){ return Factory::multiply; }

// cmultiply //////////////////////////////////////////////////////////////////

CMultiply::CMultiply(arrays in, arrays out, args a) :
    Layer(in, out, a)
{
    for(auto i : a){
        if(i == "static") fnopull = 1;
        else if(i == "abs") abs = 1;
    }
}

void CMultiply::push(){
    
    if(abs){
        for(unsigned i=0; i<left[0].csize; i++){
            float d = std::abs(left[0](i));
            for(unsigned j=0; j<left[1].csize; j++){
                right[0](i*left[1].size + j) = d * left[1](j);
            }
        }
    }
    else {
        for(unsigned i=0; i<left[0].csize; i++){
            for(unsigned j=0; j<left[1].csize; j++){
                right[0](i*left[1].size + j) = left[0](i) * left[1](j);
            }
        }
    }
}

void CMultiply::pull(){
    
    if(nopull) return;

    if(fnopull){
        if(abs){
            for(unsigned j=0; j<left[1].csize; j++){
                left[1](j) = 0.0f;
                for(unsigned i=0; i<left[0].csize; i++){
                    left[1](j) += std::abs(left[0](i)) * right[0](i*left[1].size + j);
                }
            }
        }
        else {
            for(unsigned j=0; j<left[1].csize; j++){
                left[1](j) = 0.0f;
                for(unsigned i=0; i<left[0].csize; i++){
                    left[1](j) += std::conj(left[0](i)) * right[0](i*left[1].size + j);
                }
            }
        }
    }
    else {
        if(abs){
            std::vector<std::complex<float> > d0(left[0].csize, 0.0f), d1(left[1].csize, 0.0f);
            for(unsigned i=0; i<left[0].csize; i++){
                for(unsigned j=0; j<left[1].csize; j++){
                    // don't care, implement later
                }
            }
            for(unsigned i=0; i<left[0].csize; i++) left[0](i) = d0[i];
            for(unsigned i=0; i<left[1].csize; i++) left[1](i) = d1[i];
        }
        else {
            std::vector<std::complex<float> > d0(left[0].csize, 0.0f), d1(left[1].csize, 0.0f);
            for(unsigned i=0; i<left[0].csize; i++){
                for(unsigned j=0; j<left[1].csize; j++){
                    d1[j] += std::conj(left[0](i)) * right[0](i*left[1].size + j);
                    d0[i] += std::conj(left[1](j)) * right[0](i*left[1].size + j);
                }
            }
            for(unsigned i=0; i<left[0].csize; i++) left[0](i) = d0[i];
            for(unsigned i=0; i<left[1].csize; i++) left[1](i) = d1[i];
        }
    }
}

bool CMultiply::ok(arrays in, arrays out, args a){
    if(in.size() == 2 && out.size() == 1 && in[0].size % 2 == 0 && out[0].size % 2 == 0 &&
            in[1].size % 2 == 0 && in[0].csize * in[1].csize == out[0].csize) return 1;
    return 0;
}

namespace Factory { extern std::string cmultiply; }
std::string CMultiply::get_type(){ return Factory::cmultiply; }

}

