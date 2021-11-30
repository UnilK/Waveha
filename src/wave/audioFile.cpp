#include "wave/audioFile.h"

namespace wave{

AudioFile::AudioFile(std::string fileName){
    name = fileName;
    if(!file.open(name)) badBit = 1;
}

bool AudioFile::bad(){ return badBit; }

bool AudioFile::onGetData(Chunk &data){
    
    if(badBit) return 0;


    return 1;
}

void AudioFile::onSeek(sf::Time timeOffset){
    
}

}

