#pragma once

#include "ml/layer.h"
#include "ml/judge.h"
#include "ml/db.h"

#include <vector>
#include <string>
#include <mutex>
#include <tuple>

namespace ml {

class Stack {

public:

    Stack();
    Stack(
            std::vector<std::vector<std::string> > trans,
            std::vector<std::tuple<std::string, std::string, unsigned> > links);
    ~Stack();

    bool construct(
            std::vector<std::vector<std::string> > trans,
            std::vector<std::tuple<std::string, std::string, unsigned> > links);
    
    bool construct_from_file(std::string file);
    bool good();
    
    unsigned in_size();
    unsigned out_size();

    Layer *get_layer(unsigned index);

    std::vector<float> run(const std::vector<float> &input);
    
    void train(
            const std::vector<float> &input,
            const std::vector<float> &output);

    void train_program(TrainingData &data, unsigned batchSize, unsigned batches,
            float speed, float decay);

    struct TestAnalysis {
        unsigned correct = 0;
        double error = 0;
        std::vector<double> errors;
    };

    TestAnalysis test(TrainingData &data);
    
    void apply_changes(float factor, float decay, unsigned batchSize);

    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

private:

    void clear();

    std::vector<std::vector<float> > vectors;
    std::vector<Layer*> layers;
    Judge *judge = nullptr;
    
    std::vector<std::vector<std::string> > build_trans;
    std::vector<std::tuple<std::string, std::string, unsigned> > build_links;

    std::mutex lock;

};

}

