#include "ml/waves.h"
#include "math/ft.h"
#include "change/pitch.h"
#include "wstream/wstream.h"
#include "change/detector.h"
#include "math/constants.h"
#include "ml/util.h" 

#include <filesystem>
#include <cmath>

namespace ml {

bool WaveData::open(std::string name){

    file = name;

    spectrums.open(file);

    if(spectrums.bad()) return 0;

    size = spectrums.read_unsigned();
    freqs = spectrums.read_unsigned();
    sampleSize = (2 * freqs + 1) * sizeof(float);
    begin = spectrums.tell();

    return 1;
}

InputLabel WaveData::get_random(){
    
    size_t original = pos;
    
    pos = rng32() % size;

    go_to(pos);
    auto data = get_next();
    go_to(original);

    return data;
}

InputLabel WaveData::get_next(){

    std::vector<float> data(2 * freqs + 1);

    for(float &i : data) i = spectrums.read_float();

    go_to(pos + 1);

    auto nopitch = data;
    nopitch.pop_back();

    return {data, nopitch};
}

void WaveData::go_to(size_t position){
    pos = position % size;
    spectrums.seek(begin + pos * sampleSize);
}

size_t WaveData::get_size(){ return size; }

std::string WaveData::get_file(){ return file; }

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

    const int F = 64; 

    std::vector<std::pair<std::vector<std::complex<float> >, float> > datas;

    for(auto f : files){

        iwstream I;
        if(!I.open(f)) continue;

        change::Detector detector;
        std::vector<float> samples(N);

        while(I.read_move(samples.data(), N) == N){
            
            detector.feed(samples);

            if(detector.voiced && !detector.noise && detector.confidence > 0.5f){
                
                /*
                auto clip = detector.get();
                datas.push_back({math::ft(clip, F), detector.pitch});
                */

                auto clip = detector.get2();

                float max = 0.0f;
                for(float i : clip) std::max(max, std::abs(i));
                
                if(max > 0.1f) for(float &i : clip) i /= max;
                else continue;

                int size = clip.size();
                for(int i=0; i<size; i++) clip[i] *= 0.5f - 0.5f * std::cos(2 * PIF * i / size);

                datas.push_back({math::precise_ft(clip, F, 0,
                                2.0f * (detector.real_period() / detector.period)),
                        detector.pitch});
            }

        }
    }

    ui::Saver S(output);
    if(S.bad() || datas.empty()) return 1;

    S.write_unsigned(datas.size());
    S.write_unsigned(F);

    for(auto &[i, f] : datas){
        for(auto j : i) S.write_complex(j);
        S.write_float(f);
    }

    S.close();

    return 0;
}

WaveData *wave_data(std::string file){
    
    WaveData *wd = new WaveData();
    if(!wd->open(file)){
        delete wd;
        return nullptr;
    }

    return wd;
}

}

