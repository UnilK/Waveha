#include "ml/mlml.h"
#include "ml/stack.h"
#include "ml/layer.h"

namespace ml {

CompilationResult compile(std::string file){

    /*
    auto error = [&](
            std::string error,
            unsigned line,
            std::string description) -> CompilationResult {
        
        return {
            0,
            error + " on line " + std::to_string(line),
            nullptr};
    };
    */

    return {0, "", nullptr};
}

}

