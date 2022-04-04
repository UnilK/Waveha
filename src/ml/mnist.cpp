#include "ml/mnist.h"

#include "ui/fileio.h"

#include <cassert>

namespace ml {

using std::vector;
using std::pair;

unsigned endian_flip(unsigned x){
    return (0xff000000&x<<24) | (0x00ff0000&x<<8) | (0x0000ff00&x>>8) | (0x000000ff&x>>24);
}

TrainingData *mnist_data(std::string file){
    
    ui::Loader labels(file+".lb");
    ui::Loader images(file+".img");

    if(labels.bad() || images.bad()) return nullptr;

    // we don't care about magic numbers
    labels.read_unsigned();
    images.read_unsigned();

    unsigned size = endian_flip(labels.read_unsigned());
    unsigned otherSize = endian_flip(images.read_unsigned());

    if(size != otherSize) return nullptr;

    // this is mnist dataset specific.
    unsigned imageSize = endian_flip(images.read_unsigned());
    if(imageSize != 28) return nullptr;
    imageSize = endian_flip(images.read_unsigned());
    if(imageSize != 28) return nullptr;
    
    TrainingData *p = new TrainingData(size, {vector<float>(28*28, 0.0f), vector<float>(10, 0.0f)});
    TrainingData &pairs = *p;
    
    for(auto &i : pairs){

        int label = labels.read_byte();
        assert(label >= 0 && label <= 9);
        i.second[label] = 1.0f;
        
        vector<char> image = images.read_raw(28*28);
        for(int j=0; j<28*28; j++){
            i.first[j] = (float)(unsigned char)image[j] / 255;
            assert(i.first[j] >= 0 && i.first[j] <= 1);
        }
    }

    labels.close();
    images.close();

    return p;
}

}

