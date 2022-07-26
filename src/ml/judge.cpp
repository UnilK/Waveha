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

}

