#include "change/tests.h"
#include "change/changer2.h"
#include "change/changer3.h"
#include "change/pitcher2.h"
#include "change/pitcher3.h"
#include "change/phaser.h"
#include "change/phaser2.h"
#include "change/phaser3.h"
#include "change/phaser4.h"
#include "change/resample.h"
#include "change/detector2.h"

namespace change {

std::vector<float> translate(const std::vector<float> &audio){
    
    change::Pitcher2 a(2.6, 32);
    change::Phaser4 b(44100, 200.0f, 900.0f);

    std::vector<float> result;

    for(float i : audio){
        auto j = a.process(i);
        for(float k : j) b.push(k);
        result.push_back(b.pull());
    }

    return result;
}

std::vector<float> pitchenvelope(const std::vector<float> &audio){

    change::Detector2 detector(44100, 90.0f, 500.0f);
    
    std::vector<float> pitch;
    
    int N = 128, j = 0;
    for(float i : audio){
        detector.push(i);
        if(j++%N == 0) pitch.push_back(detector.get_pitch());
    }

    return pitch;
}

}


