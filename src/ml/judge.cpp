#include "ml/judge.h"

#include <cassert>

namespace ml {

// judge //////////////////////////////////////////////////////////////////////

Judge::Judge(std::vector<float> &result_) : result(result_) {}

Judge::~Judge(){}

// average1 ///////////////////////////////////////////////////////////////////

void Average1::score(const std::vector<float> &solution){
    assert(solution.size() == result.size());
    for(unsigned i=0; i<result.size(); i++){
        result[i] = solution[i] - result[i];
        result[i] *= result[i];
    }
}

void Average1::feedback(const std::vector<float> &solution){
    assert(solution.size() == result.size());
    for(unsigned i=0; i<result.size(); i++){
        result[i] = 2 * (solution[i] - result[i]);
    }
}

namespace Factory { extern std::string average1; }
std::string Average1::get_type(){ return Factory::average1; };
bool Average1::ok(const std::vector<float> &result){ return 1; }

// average2 ///////////////////////////////////////////////////////////////////

void Average2::score(const std::vector<float> &solution){
    assert(solution.size() == result.size());
    for(unsigned i=0; i<result.size(); i+=2){
        result[i] = solution[i] - result[i];
        result[i+1] = solution[i+1] - result[i+1];
        result[i] = result[i]*result[i] + result[i+1]*result[i+1];
        result[i+1] = 0;
    }
}

void Average2::feedback(const std::vector<float> &solution){
    assert(solution.size() == result.size());
    for(unsigned i=0; i<result.size(); i++){
        result[i] = 2 * (solution[i] - result[i]);
    }
}

namespace Factory { extern std::string average2; }
std::string Average2::get_type(){ return Factory::average2; };
bool Average2::ok(const std::vector<float> &result){ return result.size()%2 == 0; }

}

