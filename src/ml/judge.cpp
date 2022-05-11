#include "ml/judge.h"

#include <complex>
#include <cassert>

namespace ml {

// judge //////////////////////////////////////////////////////////////////////

Judge::Judge(std::vector<float> &result_) : result(result_) {}

Judge::~Judge(){}

// average1 ///////////////////////////////////////////////////////////////////

void Average::score(const std::vector<float> &solution){
    assert(solution.size() == result.size());
    for(unsigned i=0; i<result.size(); i++){
        result[i] = solution[i] - result[i];
        result[i] *= result[i];
    }
}

void Average::feedback(const std::vector<float> &solution){
    assert(solution.size() == result.size());
    for(unsigned i=0; i<result.size(); i++){
        result[i] = 2 * (solution[i] - result[i]);
    }
}

namespace Factory { extern std::string average; }
std::string Average::get_type(){ return Factory::average; };
bool Average::ok(const std::vector<float> &result){ return 1; }

// phaselax ///////////////////////////////////////////////////////////////////

void Phaselax::score(const std::vector<float> &solution){
    
    assert(solution.size() == result.size());
    unsigned n = result.size() / 2;
    
    for(unsigned i=0; i<n; i++){
        std::complex<float> r = {result[2*i], result[2*i+1]}, s = {solution[2*i], solution[2*i+1]};
        if(std::abs(r) != 0.0f){
            r = ((s - r) * (float)(n-i) + (r / std::abs(r) * std::abs(s)) * (float)i) / (float)n;

        } else {
            r = s - r;
        }
        result[2*i] = std::abs(r*r);
        result[2*i+1] = 0.0f;
    }
}

void Phaselax::feedback(const std::vector<float> &solution){
    
    assert(solution.size() == result.size());
    unsigned n = result.size() / 2;

    for(unsigned i=0; i<n; i++){
        std::complex<float> r = {result[2*i], result[2*i+1]}, s = {solution[2*i], solution[2*i+1]};
        if(std::abs(r) != 0.0f){
            r = ((s - r) * (float)(n-i) + (r / std::abs(r) * std::abs(s)) * (float)i) / (float)n;
        } else {
            r = s - r;
        }
        result[2*i] = r.real();
        result[2*i+1] = r.imag();
    }
}

namespace Factory { extern std::string phaselax; }
std::string Phaselax::get_type(){ return Factory::phaselax; };
bool Phaselax::ok(const std::vector<float> &result){ return result.size()%2 == 0; }

}

