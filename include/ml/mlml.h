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

CompilationResult compile(std::string file);

}
