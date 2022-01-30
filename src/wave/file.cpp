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

void File::seek(uint32_t sample){
    file.seek(sample);
}

bool File::pull(uint32_t amount, std::vector<float> &samples){
    
    good = 1;
    
    samples = std::vector<float>(amount, 0.0f);
    
    if(file.read_move(samples, amount) != amount) good = 0;
    
    return good;
}

std::vector<float> File::get(uint32_t amount, uint32_t begin){
    return file.read_silent(begin, amount);
}

}
