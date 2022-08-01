#include "ml/waves.h"
#include "math/ft.h"
#include "change/pitch.h"
#include "wstream/wstream.h"
#include "change/detector.h"
#include "math/constants.h"
#include "ml/util.h" 

#include <filesystem>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <cassert>
#include <algorithm>

namespace ml {

WaveData::WaveData() :
    TrainingData(),
    raw(*this),
    labeled(*this),
    matched(*this)
{}

WaveData::~WaveData(){}

bool WaveData::open(std::string name){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);

    file = name;

    spectrums.open(file + ".spec");
    labels.open(file + ".lb");

    if(spectrums.bad() || labels.bad()) return 0;

    ssize = spectrums.read_unsigned();
    freqs = spectrums.read_unsigned();
    sampleSize = (2 * freqs + 1) * sizeof(float);
    sbegin = spectrums.tellg();

    lsize = labels.read_unsigned();
    lbegin = labels.tellg();
    pos = 0;

    return 1;
}

InputLabel WaveData::get(size_t position){

    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    if(pos != position % ssize) go_tog(position % ssize);

    InputLabel ret;
    ret.input.resize(2 * freqs + 1);
    ret.label.resize(64, 0.0f);

    for(float &i : ret.input) i = spectrums.read_float();

    auto label = labels.read_raw(8);
    for(unsigned i=0; i<8; i++){
        for(unsigned j=0; j<8; j++){
            if(label[i] >> j & 1) ret.label[i*8+j] = 1.0f;
        }
    }

    pos = position + 1;

    return ret;
}

size_t WaveData::size() const { return ssize; }
size_t WaveData::labeled_size(){ return lsize; }
std::string WaveData::get_file(){ return file; }
bool WaveData::has_labeled(){ return lsize > 0; }
bool WaveData::has_unlabeled(){ return lsize < ssize; }

InputLabel WaveData::next_unlabeled(){ return get(lsize); }

bool WaveData::label_next(std::vector<char> mask){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);

    assert(mask.size() == 8);

    if(lsize == ssize) return 0;
    
    go_top(lsize);
    labels.write_raw(mask.size(), mask.data());

    lsize++;
    labels.seekp(0);
    labels.write_unsigned(lsize);

    pos = ssize;

    return 1;
}

bool WaveData::discard_next(){

    std::lock_guard<std::recursive_mutex> lock(mutex);

    if(ssize == 0 || lsize == ssize) return 0;
    
    swap(lsize, ssize-1);
    ssize--;

    return 1;
}

void WaveData::shuffle(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    for(size_t i=0; i+1<lsize; i++) swap(i, i+rng32()%(lsize-i));
    for(size_t i=lsize; i+1<ssize; i++) swap(i, i+rng32()%(ssize-i));
}

void WaveData::update_index(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    for(int i=0; i<64; i++) index[i].clear();

    for(size_t i=0; i<lsize; i++){
        auto il = get(i);
        for(int j=0; j<64; j++) if(il.label[j] > 0.5f) index[j].push_back(i);
    }
}

void WaveData::swap(size_t a, size_t b){

    if(a == b) return;

    assert(a < ssize);
    assert(b < ssize);
    
    go_tog(a);
    std::vector<char> aspec(spectrums.read_raw(sampleSize));
    std::vector<char> alabe(labels.read_raw(8));
    
    go_tog(b);
    std::vector<char> bspec(spectrums.read_raw(sampleSize));
    std::vector<char> blabe(labels.read_raw(8));

    go_top(b);
    spectrums.write_raw(aspec.size(), aspec.data());
    labels.write_raw(alabe.size(), alabe.data());
    
    go_top(a);
    spectrums.write_raw(bspec.size(), bspec.data());
    labels.write_raw(blabe.size(), blabe.data());
}

void WaveData::go_tog(size_t position){
    labels.seekg(lbegin + position * 8);
    spectrums.seekg(sbegin + position * sampleSize);
    pos = position;
}

void WaveData::go_top(size_t position){
    labels.seekp(lbegin + position * 8);
    spectrums.seekp(sbegin + position * sampleSize);
}

int create_wave_data(std::string directory, std::string output, unsigned N){

    auto iswave = [](std::string &s) -> bool {
        if(s.size() < 4) return 0;
        return s.substr(s.size()-4) == ".wav";
    };

    std::vector<std::string> files;
    try {
        for(auto &file : std::filesystem::directory_iterator(directory)){
            std::string f = file.path();
            if(!iswave(f)) continue;
            files.push_back(f);
        }
    }
    catch(const std::filesystem::filesystem_error &e){
        return 1;
    }

    const unsigned F = 64; 

    ui::Saver spectrums(output+".spec"), labels(output+".lb");
    if(spectrums.bad() || labels.bad()) return 1;
    
    spectrums.write_unsigned(0);
    spectrums.write_unsigned(F);
    unsigned datas = 0;

    for(auto f : files){

        iwstream I;
        if(!I.open(f)) continue;

        change::Detector detector;
        std::vector<float> samples(N);

        while(I.read_move(samples.data(), N) == N){
            
            detector.feed(samples);

            if(detector.voiced){
                
                unsigned f = std::min(F, (unsigned)std::floor(
                            std::min((float)I.get_frame_rate() / 2, 7500.0f)
                            / detector.pitch));
                auto freq = math::cos_window_ft(detector.get2(), f);
                freq.resize(F, 0.0f);

                float sum = 0.0f;
                for(unsigned i=0; i<F; i++) sum += (freq[i]*std::conj(freq[i])).real();
                sum = std::sqrt(sum);

                if(sum < 1e-9) continue;

                sum = 1.0f / sum;
                for(auto &i : freq) i *= sum;
                
                for(auto i : freq) spectrums.write_complex(i);
                spectrums.write_float(detector.pitch);

                datas++;
            }
        }
    }

    spectrums.seek(0);
    spectrums.write_unsigned(datas);
    
    labels.write_unsigned(0);
    
    char noLabel[8] = {0};
    for(unsigned i=0; i<datas; i++) labels.write_raw(8, noLabel);

    spectrums.close();
    labels.close();

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

namespace _merge_wave_data {

Input::Input(std::string s, std::string l) :
    spectrums(s),
    labels(l)
{}

}

int merge_wave_data(std::string directory, std::string output){

    using namespace _merge_wave_data;

    std::vector<Input* > inputs;

    {
        auto isspec = [](std::string &s) -> bool {
            if(s.size() < 5) return 0;
            return s.substr(s.size()-5) == ".spec";
        };
        
        auto islabe = [](std::string &s) -> bool {
            if(s.size() < 3) return 0;
            return s.substr(s.size()-3) == ".lb";
        };

        std::vector<std::string> spectrums, labels;
        try {
            for(auto &file : std::filesystem::directory_iterator(directory)){
                std::string f = file.path();
                if(isspec(f)) spectrums.push_back(f);
                if(islabe(f)) labels.push_back(f);
            }
        }
        catch(const std::filesystem::filesystem_error &e){
            return 1;
        }

        std::sort(spectrums.begin(), spectrums.end());
        std::sort(labels.begin(), labels.end());

        if(spectrums != labels || labels.empty()) return 1;
    
        for(unsigned i=0; i<labels.size(); i++){
            Input *in = new Input(spectrums[i], labels[i]);
            if(in->spectrums.bad() || in->labels.bad()) return 1;
            in->ssize = in->spectrums.read_unsigned();
            in->freqs = in->spectrums.read_unsigned();
            in->lsize = in->labels.read_unsigned();
            inputs.push_back(in);
        }

        unsigned freqs = inputs[0]->freqs;
        for(Input *i : inputs) if(i->freqs != freqs) return 1;
    }

    
    ui::Saver ospec(output+".spec"), olabe(output+".lb");
    if(ospec.bad() || olabe.bad()) return 3;
    
    unsigned ssize = 0, lsize = 0;
    unsigned freqs = inputs[0]->freqs;
    unsigned sampleSize = (2 * freqs + 1) * sizeof(float);

    for(Input *i : inputs) ssize += i->ssize;
    for(Input *i : inputs) lsize += i->lsize;

    ospec.write_unsigned(ssize);
    ospec.write_unsigned(freqs);
    olabe.write_unsigned(lsize);

    std::vector<char> bus;

    for(Input *i : inputs){
        bus = i->spectrums.read_raw(i->lsize*sampleSize);
        ospec.write_raw(bus.size(), bus.data());
        
        bus = i->labels.read_raw(i->lsize*8);
        olabe.write_raw(bus.size(), bus.data());
    }

    for(Input *i : inputs){
        bus = i->spectrums.read_raw((i->ssize-i->lsize)*sampleSize);
        ospec.write_raw(bus.size(), bus.data());
        
        bus = i->labels.read_raw((i->ssize-i->lsize)*8);
        olabe.write_raw(bus.size(), bus.data());
    }

    for(Input *i : inputs) delete i;
    
    ospec.close();
    olabe.close();

    return 0;
}

// raw ///////////////////////////////////////////////////////////////////////

WaveData::Raw::Raw(WaveData &source) : 
    src(source)
{}

InputLabel WaveData::Raw::get(size_t position){
    auto ret = src.get(position);
    ret.label = ret.input;
    ret.label.pop_back();
    return ret;
}

size_t WaveData::Raw::size() const { return src.ssize; }
    
// labeled ////////////////////////////////////////////////////////////////////

WaveData::Labeled::Labeled(WaveData &source) :
    src(source)
{}

InputLabel WaveData::Labeled::get(size_t position){
    return src.get(position % src.lsize);
}

size_t WaveData::Labeled::size() const { return src.lsize; }

// matched ////////////////////////////////////////////////////////////////////

WaveData::Matched::Matched(WaveData &source) :
    src(source),
    sameProb(0.5f)
{}

InputLabel WaveData::Matched::get(size_t position){

    std::lock_guard<std::recursive_mutex> lock(src.mutex);

    for(auto &i : src.index){
        if(i.size() > position){
            
            auto apos = i[position];
            auto bpos = (float)rng32()/(1ll<<32) < sameProb ? apos : i[rng32()%i.size()];

            InputLabel a = src.get(apos);
            InputLabel b;
            if(bpos != apos) b = src.get(bpos);
            else b = a;

            InputLabel ret;
            ret.input = a.input;
            ret.label = b.input;
            ret.input.back() = ret.label.back() / ret.input.back();
            ret.label.pop_back();

            return ret;
        }
        position -= i.size();
    }

    return {{}, {}};
}

size_t WaveData::Matched::size() const { return src.lsize; }

void WaveData::Matched::match_same(float probability){
    sameProb = std::min(1.0f, std::max(0.0f, probability));
}

}

