#include "wave/file.h"
#include "wave/util.h"

#include <algorithm>
#include <string.h>

namespace wave {

File::File(){}

File::File(std::string fileName){
    open(fileName);
}

File::~File(){}

void File::open(std::string fileName){
    
    good = 1;
    name = fileName;
    
    if(file.open(name)){
        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
    }
    else good = 0;
}

void File::seek(unsigned sample){
    file.seek(sample);
}

unsigned File::tell(){
    return file.tell();
}

unsigned File::size(){
    return file.get_sample_amount();
}

unsigned File::pull(unsigned amount, std::vector<float> &samples){
    
    good = 1;
    
    samples = std::vector<float>(amount, 0.0f);
    
    unsigned actual = file.read_move(samples, amount);

    if(actual != amount) good = 0;
    
    return actual;
}

std::vector<float> File::get(unsigned amount, unsigned begin){
    return file.read_silent(begin, amount);
}

}
