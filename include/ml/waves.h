#pragma once

#include "ml/db.h"
#include "ui/fileio.h"

#include <string>
#include <vector>

namespace ml {

class WaveData : public TrainingData {

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
    ui::Loader spectrums;

    size_t begin, size, pos, freqs, sampleSize;

};

int create_wave_data(std::string directory, std::string output, unsigned N);

WaveData *wave_data(std::string file);

}

