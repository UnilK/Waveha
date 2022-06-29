#include "ml/mnist.h"

#include "ui/fileio.h"
#include "math/fft.h"
#include "ml/util.h"

#include <cassert>

namespace ml {

using std::vector;
using std::pair;

unsigned endian_flip(unsigned x){
    return (0xff000000&x<<24) | (0x00ff0000&x<<8) | (0x0000ff00&x>>8) | (0x000000ff&x>>24);
}

bool MnistData::open(std::string name){

    file = name;
    bool ok = labels.open(file+".lb") && images.open(file+".img");

    if(!ok) return 0;
    
    ok &= endian_flip(labels.read_unsigned()) == 2049;
    ok &= endian_flip(images.read_unsigned()) == 2051;

    if(!ok) return 0;

    size = endian_flip(labels.read_unsigned());
    ok &= size == endian_flip(images.read_unsigned());
    
    if(!ok) return 0;

    imgw = endian_flip(images.read_unsigned()); 
    imgh = endian_flip(images.read_unsigned());
    pos = 0;

    lbegin = labels.tell();
    ibegin = images.tell();

    return 1;
}

InputLabel MnistData::get_random(){
    
    size_t original = pos;
    
    pos = rng32() % size;

    go_to(pos);
    auto data = get_next();
    go_to(original);

    return data;
}

InputLabel MnistData::get_next(){

    InputLabel data;
    data.input.resize(imgw*imgh, 0.0f);
    data.label.resize(10, 0.0f);

    int num = labels.read_byte();
    data.label[num] = 1.0f;
    
    vector<char> buff = images.read_raw(imgw*imgh);
    for(int j=0; j<imgw*imgh; j++)
        data.input[j] = (float)(unsigned char)buff[j] / 255;

    go_to(pos + 1);

    return data;
}

void MnistData::go_to(size_t position){
    
    pos = position % size;
    images.seek(ibegin + pos * imgw * imgh);
    labels.seek(lbegin + pos);

}

size_t MnistData::get_size(){ return size; }

std::string MnistData::get_file(){ return file; }

void MnistData::blur(){
    
    size_t previousPos = pos;
    go_to(0);

    vector<vector<float> > kernel = {
        {0.1, 0.3, 0.1},
        {0.3, 1.0, 0.3},
        {0.1, 0.3, 0.1},
    };
    
    ui::Saver out(file + ".img");

    for(size_t iter = size; iter --> 0;){
        
        size_t here = images.tell();

        auto [matrix, label] = get_next();
       
        float osum = 0.0f;
        for(float i : matrix) osum += i;

        vector<float> conv(imgw*imgh, 0.0f);

        for(int i=0; i<imgw; i++){
            for(int j=0; j<imgh; j++){
                for(int ii=-1; ii<2; ii++){
                    for(int jj=-1; jj<2; jj++){
                        if(i+ii < 0 || i+ii >= imgw || j+jj < 0 || j+jj >= imgh) continue;
                        conv[(i+ii)*imgh+j+jj] += matrix[i*imgh+j] * kernel[ii+1][jj+1];
                    }
                }
            }
        }

        float csum = 0.0f;
        for(float i : conv) csum += i;

        float diff = osum / csum;
        for(float &i : conv) i *= diff; 

        vector<unsigned char> buff(imgw*imgh);
        for(int j=0; j<imgw*imgh; j++)
            buff[j] = (unsigned char)std::min(255.0f, conv[j] * 255);

        out.seek(here);
        out.write_raw(buff.size(), buff.data());
    }

    out.close();

    go_to(previousPos);
}

MnistData *mnist_data(std::string file){
    
    MnistData *md = new MnistData();
    if(!md->open(file)){
        delete md;
        return nullptr;
    }

    return md;
}

}

