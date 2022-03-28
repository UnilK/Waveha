#pragma once

#include "ml/layer.h"

#include <vector>
#include <string>

namespace ml {

typedef std::pair<std::vector<float>, std::vector<float> > InputLabel;

class Stack {

public:

    Stack();
    Stack(std::vector<unsigned> sizes, std::vector<std::string> types);
    ~Stack();

    bool construct(std::vector<unsigned> sizes, std::vector<std::string> types);
    bool construct_from_file(std::string file);
    bool good();
    
    Layer *get_layer(unsigned index);

    std::vector<float> run(const std::vector<float> &input);
    
    void train(
            const std::vector<float> &input,
            const std::vector<float> &output);

    void train_progam(const std::vector<InputLabel > &data, unsigned batches);

    struct TestAnalysis {
        unsigned correct = 0;
        double error = 0;
        std::vector<double> errors;
    };

    TestAnalysis test(const std::vector<InputLabel > &data);
    
    void set_batch_size(unsigned);
    void set_speed(double);
    void apply_changes();

    void save(app::Saver &saver);
    void load(app::Loader &loader);

private:

    void clear();

    unsigned batchSize = 100;
    double batch = 0, speed = 0.1;

    std::vector<std::vector<float> > vectors;
    std::vector<Layer*> layers;
    Layer *evaluate = nullptr;

};

}

