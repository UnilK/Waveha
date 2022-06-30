#include "ml/mlml.h"
#include "ml/stack.h"
#include "ml/layer.h"

namespace ml {

CompilationResult error(
        std::string error,
        unsigned line,
        std::string description){
    return {0, "", nullptr};
}

CompilationResult compile(std::string file){
    return {0, "", nullptr};
}

}

