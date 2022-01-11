#include "wave/file.h"
#include "wave/util.h"

#include <algorithm>
#include <string.h>

namespace wave{

File::File(){}

File::File(std::string fileName){
    open(fileName);
}

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

std::vector<float> File::pull(uint32_t amount){
    
    good = 1;
    
    std::vector<float> audio;
    
    if(file.read_move(audio, amount) != amount) good = 0;
    
    return audio;
}

std::vector<float> File::get(uint32_t amount, uint32_t begin){
    return file.read_silent(begin, amount);
}

}
