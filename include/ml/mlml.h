#pragma once

#include <string>

namespace ml {

class Stack;
class Layer;

struct CompilationResult {

    bool success;
    std::string message;
    Stack *stack;

};

struct LayerResult {

    bool success;
    std::string message;
    Layer *layer;

};

CompilationResult error(
        std::string error,
        unsigned line,
        std::string description);

CompilationResult compile(std::string file);

}
