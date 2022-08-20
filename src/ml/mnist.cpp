#include "ml/mnist.h"

#include "ui/fileio.h"
#include "math/fft.h"
#include "ml/util.h"

#include <cassert>
#include <iostream>

namespace ml {

using std::vector;
using std::pair;

unsigned endian_flip(unsigned x){
    return (0xff000000&x<<24) | (0x00ff0000&x<<8) | (0x0000ff00&x>>8) | (0x000000ff&x>>24);
}

bool MnistData::open(std::string name){

    std::lock_guard<std::recursive_mutex> lock(mutex);

    file = name;
    bool ok = labels.open(file+".lb") && images.open(file+".img");

    if(!ok) return 0;
    
    ok &= endian_flip(labels.read_unsigned()) == 2049;
    ok &= endian_flip(images.read_unsigned()) == 2051;

    if(!ok) return 0;

    msize = endian_flip(labels.read_unsigned());
    ok &= msize == endian_flip(images.read_unsigned());
    
    if(!ok) return 0;

    imgh = endian_flip(images.read_unsigned()); 
    imgw = endian_flip(images.read_unsigned());
    pos = 0;

    lbegin = labels.tell();
    ibegin = images.tell();

    return 1;
}

InputLabel MnistData::direct_get(size_t position){

    std::lock_guard<std::recursive_mutex> lock(mutex);
   
    if(msize == 0) return {{}, {}};

    if(position % msize != pos){
        pos = position % msize;
        images.seek(ibegin + pos * imgh * imgw);
        labels.seek(lbegin + pos);
    }

    InputLabel data;
    data.input.resize(imgh*imgw, 0.0f);
    data.label.resize(10, 0.0f);

    int num = labels.read_byte();
    data.label[num] = 1.0f;
    
    const int pic = imgh * imgw;
    vector<char> buff = images.read_raw(pic);
    
    for(int j=0; j<pic; j++)
        data.input[j] = (float)(unsigned char)buff[j] / 255;

    /*
    if((rng32()&((1<<15)-1)) == 0){
        std::cout << num << '\n';
        for(int i=0, k=0; i<imgh; i++){
            for(int j=0; j<imgw; j++, k++){
                if(data.input[k] > 0.7f) std::cout << '#';
                else if(data.input[k] > 0.4f) std::cout << 'o';
                else std::cout << '.';
            } std::cout << '\n';
        } std::cout << '\n';
    }
    */

    pos++;

    return data;
}

size_t MnistData::size() const { return msize; }

std::string MnistData::get_file(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);

    return file;
}

void MnistData::blur(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    vector<vector<float> > kernel = {
        {0.1, 0.3, 0.1},
        {0.3, 1.0, 0.3},
        {0.1, 0.3, 0.1},
    };
    
    ui::Saver out(file + ".img");

    for(size_t iter = 0; iter < msize; iter++){
        
        size_t here = images.tell();

        auto [matrix, label] = get(iter);
       
        float osum = 0.0f;
        for(float i : matrix) osum += i;

        vector<float> conv(imgh*imgw, 0.0f);

        for(int i=0; i<imgh; i++){
            for(int j=0; j<imgw; j++){
                for(int ii=-1; ii<2; ii++){
                    for(int jj=-1; jj<2; jj++){
                        if(i+ii < 0 || i+ii >= imgh || j+jj < 0 || j+jj >= imgw) continue;
                        conv[(i+ii)*imgw+j+jj] += matrix[i*imgw+j] * kernel[ii+1][jj+1];
                    }
                }
            }
        }

        float csum = 0.0f;
        for(float i : conv) csum += i;

        float diff = osum / csum;
        for(float &i : conv) i *= diff; 

        vector<unsigned char> buff(imgh*imgw);
        for(int j=0; j<imgh*imgw; j++)
            buff[j] = (unsigned char)std::min(255.0f, conv[j] * 255);

        out.seek(here);
        out.write_raw(buff.size(), buff.data());
    }

    out.close();
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

