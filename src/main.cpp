#include "math/FT.h"
#include "math/FFT.h"
#include "wave/waveTransform.h"
#include "wave/pitchHandler.h"
#include "wave/audioClassifier.h"

#include "wavestream.h"

#include <bits/stdc++.h>
using namespace std;

int main(){

    iwavestream I;

    string filename;
    cin >> filename;

    while(!I.open(filename)){
        cout << "that's not a wav file you clumsy oaf\n";
        cin >> filename;
    }

    int32_t freq = I.frameRate;

    vector<float> idata, odata;
    I.read_file(idata);

    int size = idata.size();
    int len = 1<<11;
    float *waves = idata.data();

    for(int i=0; i<size-len; i+=len/2){
        float fund = wave::find_pitch(waves+i, len, freq, 80, 200, 7);
        int32_t wavel = (int32_t)std::ceil(freq/fund);
        cout << i << " -- " << fund << ' ' << wavel << " -> ";
        wavel = wave::best_pitch_length(waves+i, len, wavel, 1.25f);
        fund = (float)freq/wavel;
        cout << fund << ' ' << wavel << "\n";
        cout << wave::is_noise(waves+i, len, 1)
            << ' ' << wave::is_silent(waves+i, len, 0.01f) << '\n';
    }

    /*
    owavestream O("eek.wav", &I);

    O.write_file(odata);
    I.close();
    */

    return 0;
}
