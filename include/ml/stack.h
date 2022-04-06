#pragma once

#include "ml/layer.h"

#include <vector>
#include <string>
#include <mutex>

namespace ml {

typedef std::pair<std::vector<float>, std::vector<float> > InputLabel;
typedef std::vector<InputLabel > TrainingData;

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

    void train_program(const TrainingData &data, unsigned batchSize, unsigned batches, double speed);

    struct TestAnalysis {
        unsigned correct = 0;
        double error = 0;
        std::vector<double> errors;
    };

    TestAnalysis test(const TrainingData &data);
    
    void apply_changes(double);

    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

private:

    void clear();

    double batch = 0;

    std::vector<std::vector<float> > vectors;
    std::vector<Layer*> layers;
    Layer *evaluate = nullptr;

    std::mutex lock;

};

}

