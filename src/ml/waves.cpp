#include "ml/waves.h"
#include "math/ft.h"
#include "change/pitch.h"
#include "wstream/wstream.h"
#include "ui/fileio.h"

#include <filesystem>

namespace ml {

const int N = 1<<11;

int create_wave_data(std::string directory, std::string output, unsigned N){

    auto iswave = [](std::string &s) -> bool {
        if(s.size() < 4) return 0;
        return s.substr(s.size()-4) == ".wav";
    };

    std::vector<std::string> files;
    for(auto &file : std::filesystem::directory_iterator(directory)){
        std::string f = file.path();
        if(!iswave(f)) continue;
        files.push_back(f);
    }

    const int M = 1<<11;
    const int F = 64; 

    std::vector<std::vector<std::complex<float> > > datas;

    for(auto f : files){

        iwstream I(f);

        std::vector<float> waves = I.read_file();

        unsigned step = (waves.size()-M) / N;

        for(unsigned i=0; i<N && i*step+M < waves.size(); i++){

            std::vector<float> clip(M, 0.0f);
            for(unsigned j=0; j<M; j++)  clip[j] = waves[i*step+j];

            unsigned length = change::pitch(clip);
            clip.resize(length);

            auto freqs = math::ft(clip, F);

            float sum = 0.0f;
            for(auto &i : freqs) sum += std::abs(i);
            if(sum != 0.0f) sum = freqs.size() / sum;
            for(auto &i : freqs) i *= sum;

            datas.push_back(freqs);
        }
    }

    ui::Saver S(output);
    if(S.bad() || datas.empty()) return 1;

    S.write_unsigned(datas.size());
    S.write_unsigned(F);

    for(auto &i : datas) for(auto j : i) S.write_complex(j);

    S.close();

    return 0;
}

TrainingData *wave_data(std::string file){
    
    ui::Loader L(file);

    if(L.bad()) return nullptr;

    TrainingData &data = *(new TrainingData());

    unsigned size = L.read_unsigned();
    unsigned f = L.read_unsigned();

    for(unsigned i=0; i<size; i++){
        data.push_back({std::vector<float>(2*f), {}});
        for(unsigned j=0; j<2*f; j++) data.back().first[j] = L.read_float();
        data.back().second = data.back().first;
    }

    return &data;
}

}

