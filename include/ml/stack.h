#pragma once

#include "ml/layer.h"

#include <complex>
#include <vector>
#include <string>

namespace ml {

class Stack {

public:

    Stack();
    Stack(std::vector<unsigned> sizes, std::vector<std::string> types);
    ~Stack();

    bool open(std::vector<unsigned> sizes, std::vector<std::string> types);
    bool good();
    
    Layer *get_layer(unsigned index);

    std::vector<std::complex<float> > run(const std::vector<std::complex<float> > &input);
    
    void train(
            const std::vector<std::complex<float> > &input,
            const std::vector<std::complex<float> > &output);
    
    double score(
            const std::vector<std::complex<float> > &input,
            const std::vector<std::complex<float> > &output);

    void set_speed(double);
    void apply_changes();

    void save(app::Saver &saver);
    void load(app::Loader &loader);

private:

    void clear();

    double batch = 0, speed = 0.1;

    std::vector<std::vector<std::complex<float> > > vectors;
    std::vector<Layer*> layers;
    Layer *evaluate = nullptr;

};

}

