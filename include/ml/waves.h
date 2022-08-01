#pragma once

#include "ml/db.h"
#include "ui/fileio.h"

#include <string>
#include <vector>

namespace ml {

class WaveData : public TrainingData {

public:

    WaveData();
    ~WaveData();
    
    bool open(std::string name);

    InputLabel get(size_t position);
    size_t size() const;
    size_t labeled_size();

    std::string get_file();

    bool has_labeled();
    bool has_unlabeled();
    InputLabel next_unlabeled();
    bool label_next(std::vector<char> mask);
    bool discard_next();

    void shuffle();
    
    void update_index();
   
    // sockets for accessing the data

    class Raw : public TrainingData {
    public:
        Raw(WaveData&);
        InputLabel get(size_t position);
        size_t size() const;
    private:
        WaveData &src;
    };
    
    class Labeled : public TrainingData {
    public:
        Labeled(WaveData&);
        InputLabel get(size_t position);
        size_t size() const;
    private:
        WaveData &src;
    };
    
    class Matched : public TrainingData {
    public:
        Matched(WaveData&);
        InputLabel get(size_t position);
        size_t size() const;
        void match_same(float probability);
    private:
        WaveData &src;
        float sameProb;
    };
    
    Raw raw;
    Labeled labeled;
    Matched matched;

private:

    void swap(size_t, size_t);
    void go_tog(size_t);
    void go_top(size_t);

    std::string file;
    ui::Editor spectrums, labels;

    size_t sbegin, ssize, freqs, sampleSize;
    size_t lbegin, lsize;
    size_t pos;

    std::vector<std::vector<size_t> > index;

    std::recursive_mutex mutex;
};

int create_wave_data(std::string directory, std::string output, unsigned N);

WaveData *wave_data(std::string file);

namespace _merge_wave_data {

struct Input {
    Input(std::string, std::string);
    ui::Loader spectrums, labels;
    unsigned ssize;
    unsigned freqs;
    unsigned lsize;
};

}

int merge_wave_data(std::string directory, std::string output);

}

