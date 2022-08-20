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

    InputLabel direct_get(size_t position);
    size_t size() const;

    std::string get_file();

    void blur();

private:

    std::string file;
    ui::Loader images, labels;

    size_t msize, pos;
    int imgw, imgh;
    size_t lbegin, ibegin;

};

MnistData *mnist_data(std::string file);

}

