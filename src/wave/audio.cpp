#include "wave/audio.h"

#include "wstream/wstream.h"

namespace wave {

Audio::Audio(){}
    
Audio::Audio(    
    std::string name_,
    unsigned channels_,
    unsigned frameRate_,
    const std::vector<float> &data_) :
    name(name_),
    channels(channels_),
    frameRate(frameRate_),
    data(data_)
{}

Audio::Audio(const Audio &other) :
    name(other.name),
    channels(other.channels),
    frameRate(other.frameRate),
    data(other.data)
{}

Audio::Audio(std::string fileName){
    open(fileName);
}

Audio::~Audio(){}

bool Audio::open(std::string fileName){

    iwstream file;

    if(file.open(fileName)){
        
        name = fileName;

        frameRate = file.get_frame_rate();
        channels = file.get_channel_amount();
        
        data = file.read_file();
        
        return 1;
    }

    return 0;
}

}

