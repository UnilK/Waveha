#include "ml/ft.h"
#include "math/ft.h"

namespace ml {

FT::FT(std::vector<float> &source, std::vector<float> &destination) :
    Layer(source, destination),
    freq(destination.size()/2)
{}

void FT::push(){
    freq = math::ft(left, right.size()/2);
    for(unsigned i=0; i<freq.size(); i++){
        right[2*i] = freq[i].real();
        right[2*i+1] = freq[i].imag();
    }
}

void FT::pull(){
    /*
    for(unsigned i=0; i<freq.size(); i++) freq[i] = {right[2*i], right[2*i+1]};
    left = math::ift(freq, left.size());
    */
}

bool FT::ok(std::vector<float> &left, std::vector<float> &right){
    return right.size()%2 == 0;
}

namespace Factory { extern std::string ft; };
std::string FT::get_type(){ return Factory::ft; }

}

