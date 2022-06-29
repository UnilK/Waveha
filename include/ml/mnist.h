#pragma once

#include "ml/db.h"
#include "ui/fileio.h"

#include <string>
#include <vector>
#include <fstream>

namespace ml {

unsigned endian_flip(unsigned);

class MnistData : public TrainingData {

public:

    using TrainingData::TrainingData;

    bool open(std::string name);

    InputLabel get_random();
    InputLabel get_next();
    void go_to(size_t position = 0);
    size_t get_size();

    std::string get_file();

    void blur();

private:

    std::string file;
    ui::Loader images, labels;

    size_t size, pos;
    int imgw, imgh;
    size_t lbegin, ibegin;

};

MnistData *mnist_data(std::string file);

}

