#include "ml/control.h"

#include <cmath>

namespace ml {

Control::Control(arrays in, arrays out, args a) :
    Layer(in, out, a)
{}

void Control::push(){}
void Control::pull(){}

namespace Factory { extern std::string control; }
std::string Control::get_type(){ return Factory::control; }

bool Control::ok(arrays in, arrays out, args a){
    return in.size() == 1 && out.size() == 1 && in[0].size == out[0].size;
}

std::string Control::get_name(){ return name; }
void Control::set(float m, float b){}
void Control::set_back(){}

// pitchlog ///////////////////////////////////////////////////////////////////

PitchLog::PitchLog(arrays in, arrays out, args a) :
    Layer(in, out, a)
{
    std::vector<float> nums;
    for(auto i : a){
        try {
            nums.push_back(std::stof(i));
        } catch (const std::invalid_argument &e){}
    }
    add = nums[0];
    mul = nums[1];
}

void PitchLog::push(){
    for(unsigned i=0; i<left[0].size; i++)
        right[0][i] = left[0][i] * mul + add;
}

void PitchLog::pull(){}

namespace Factory { extern std::string pitchlog; }
std::string PitchLog::get_type(){ return Factory::pitchlog; }

bool PitchLog::ok(arrays in, arrays out, args a){
    
    std::vector<float> nums;
    for(auto i : a){
        try {
            nums.push_back(std::stof(i));
        } catch (const std::invalid_argument &e){}
    }
    
    return in.size() == 1 && out.size() == 1 && in[0].size == out[0].size
        && nums.size() == 2 && nums[1] != 0.0f;
}

}

