#include "ml/mnist.h"

#include "app/session.h"

namespace ml {

using std::vector;
using std::pair;

unsigned endian_flip(unsigned x){
    return (0xff000000&x<<24) | (0x00ff0000&x<<8) | (0x0000ff00&x>>8) | (0x000000ff&x>>24);
}

vector<InputLabel > mnist_input_label_pairs(std::string file){
    
    app::Loader labels(file+".lb");
    app::Loader images(file+".img");

    if(labels.bad() || images.bad()) return {{{}, {}}};

    // we don't care about magic numbers
    labels.read_unsigned();
    images.read_unsigned();

    unsigned size = endian_flip(labels.read_unsigned());
    unsigned otherSize = endian_flip(images.read_unsigned());

    if(size != otherSize) return {{{}, {}}};

    // this is mnist dataset specific.
    unsigned imageSize = endian_flip(images.read_unsigned());
    if(imageSize != 28) return {{{}, {}}};
    imageSize = endian_flip(images.read_unsigned());
    if(imageSize != 28) return {{{}, {}}};

    vector<InputLabel > pairs(size, {vector<float>(28*28, 0.0f), vector<float>(10, 0.0f)});

    for(auto &i : pairs){
        
        int label = labels.read_byte();
        vector<char> image = images.read_raw(28*28);
        
        i.second[label] = 1.0f;
        for(int j=0; j<28*28; j++) i.first[j] = (float)(unsigned char)image[j] / 255;
    }

    return pairs;
}

}

