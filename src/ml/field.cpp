#include "ml/field.h"

#include <assert.h>

namespace ml {

Field::Field(
        std::vector<std::complex<float> > &source,
        std::vector<std::complex<float> > &destination) :
    Layer(source, destination)
{
    assert(left.size() == right.size());
}

void Field::push(){

    for(unsigned i=0; i<left.size(); i++){

        float abs = std::abs(left[i]);

        if(abs < 1.0f){
            right[i] = left[i] * abs * 0.5f;
        }
        else {
            right[i] = (1.0f - 1.0f / (std::conj(left[i]) * abs)) * 0.5f;
        }
    }
}

void Field::pull(){

    auto dot = [&](
            const std::complex<float> &a,
            const std::complex<float> &b) -> float{
        return a.real()*b.real() + a.imag()*b.imag();
    };

    for(unsigned i=0; i<left.size(); i++){

        float abs = std::abs(left[i]);

        if(abs < 1.0f){
            // gradient = left[i]
            left[i] = dot(left[i], right[i]) * right[i] / std::abs(right[i]);
        }
        else {
            auto gradient = 1.0f / (std::conj(left[i]) * abs * abs);
            left[i] = std::abs(dot(gradient, right[i])) * right[i] / std::abs(right[i]);
        }
    }
}

std::string Field::get_type(){ return type; };

const std::string Field::type = "field";

}

