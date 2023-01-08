#pragma GCC target("avx2")
#pragma GCC optimize("O3")
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
#include "math/fft.h"
#include "math/constants.h"
#include "change/util.h"

#include <complex>
#include <random>
#include <iostream>

namespace change {

std::vector<float> translate(const std::vector<float> &audio){
    
    change::Pitcher2 a(2.1, 32);
    change::Phaser4 b(44100, 100.0f, 400.0f);

    std::vector<float> result;

    for(float i : audio){
        auto j = a.process(i);
        for(float k : j) b.push(k);
        result.push_back(b.pull());
    }

    /*
    const int N = 256;

    auto filtered = result;
    int n = filtered.size();
    for(float &i : result) i = 0.0f;

    std::vector<float> window(N);
    for(int i=0; i<N; i++) window[i] = (std::cos(2*PI*i/N) - 1.0) / sqrt(6);

    for(int i=0; i+N<n; i+=N/4){
        std::vector<float> bit(N);
        for(int j=0; j<N; j++) bit[j] = filtered[i+j] * window[j];
        auto freq = math::fft(bit);
        for(int j=0; j<=N-j; j++){
            int k = (N-j)%N;
            std::complex<float> a = std::polar(1.0f, (float)(2 * PI * (rnd()*0.4-0.2)));
            freq[j] *= a;
            freq[k] *= std::conj(a);
        }
        bit = math::inverse_fft(freq);
        for(int j=0; j<N; j++) result[i+j] += bit[j] * window[j];
    }
    */

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

std::vector<float> frequencywindow(){
    
    using std::vector;
    using std::tuple;
    using std::complex;

    static unsigned f = 0;
    vector<tuple<int, float, float> > wavelets;

    {
        double freq = 160.0;
        double len = 44100.0 / 80.0;
        double fmul = 1.15;
        double lmul = 1.0 / 1.105;

        {
            int n = std::round(len/4)*4;
            wavelets.push_back({n, 0/44100.0*n, 1.2*std::log(10.0)});
            wavelets.push_back({n, 40/44100.0*n, 1.55*std::log(40)});
            wavelets.push_back({n, 80/44100.0*n, 1.77*std::log(80)});
            wavelets.push_back({n, 120/44100.0*n, 1.1*std::log(120)});
            wavelets.push_back({n, 140/44100.0*n, 1.05*std::log(140)});
        }

        while(freq < 6000.0){
            int n = std::round(len/4)*4;
            wavelets.push_back({n, freq/44100.0*n, std::log(freq)});
            freq *= fmul;
            len *= lmul;
        }
    }

    const int n = 1<<12;
    const int N = 1<<10;

    static vector<vector<float> > sums;
    sums.resize(wavelets.size());
    for(auto &i : sums) i.resize(n, 0.0f);
    
    vector<float> enve(n, 0.0f), audio(n);

    if(f == wavelets.size()){
        for(auto i : sums){
            for(int j=0; j<n; j++) enve[j] += i[j];
        }
    } else {
        auto [m, d, h] = wavelets[f];
        vector<complex<float> > window(m);
        for(int i=0; i<m; i++){
            float len = 1.0f-std::cos(2*PIF*i/m);
            window[i] = std::polar(len, -2*PIF*i*d/m);
        }

        {
            float sum = 0.0f;
            for(auto i : window) sum += std::norm(i);
            sum = std::sqrt(sum);
            for(auto &i : window) i /= sum;
        }

        std::cerr << m << ' ' << d << ' ' << h << " -> ";

        for(int x=0; x<N; x++){
            for(float &i : audio) i = 2.0 * rnd() - 1.0;
            std::vector<float> result(n, 0.0f);
            for(int i=0; i+m<n; i+=m/4){
                complex<float> sum = 0.0f;
                for(int j=0; j<m; j++) sum += audio[i+j] * window[j];
                sum *= h * 2;
                for(int j=0; j<m; j++) result[i+j] += (sum*std::conj(window[j])).real();
            }
            for(int i=0; i<n; i++) result[i] *= (1.0f - std::cos(2 * PIF * i / n)) * 0.5f;
            auto freq = math::fft(result);
            for(int i=0; i<n; i++) enve[i] += std::norm(freq[i]);
        }

        std::cerr << "done\n";

        for(float &i : enve) i /= N * n;

        sums[f] = enve;
    }

    f = (f+1)%(wavelets.size()+1);
    
    return enve;
}

std::vector<float> hodgefilter(const std::vector<float> &audio){

    using std::vector;
    using std::tuple;
    using std::complex;

    vector<tuple<int, float, float> > wavelets;

    {
        double freq = 160.0;
        double len = 44100.0 / 80.0;
        double fmul = 1.15;
        double lmul = 1.0 / 1.15;

        {
            int n = std::round(len/4)*4;
            wavelets.push_back({n, 0/44100.0*n, 1.2*std::log(10.0)});
            wavelets.push_back({n, 40/44100.0*n, 1.55*std::log(40)});
            wavelets.push_back({n, 80/44100.0*n, 1.77*std::log(80)});
            wavelets.push_back({n, 120/44100.0*n, 1.1*std::log(120)});
            wavelets.push_back({n, 140/44100.0*n, 1.05*std::log(140)});
        }

        while(freq < 16000.0){
            int n = std::round(len/4)*4;
            wavelets.push_back({n, freq/44100.0*n, std::log(freq)});
            freq *= fmul;
            len *= lmul;
        }
    }

    const int n = audio.size();

    vector<float> result(n, 0.0f);
    vector<vector<complex<float> > > windows;

    for(auto [m, d, h] : wavelets){
        vector<complex<float> > window(m);
        for(int i=0; i<m; i++){
            float len = 1.0f-std::cos(2*PIF*i/m);
            window[i] = std::polar(len, -2*PIF*i*d/m);
        }
        float sum = 0.0f;
        for(auto i : window) sum += std::norm(i);
        sum = std::sqrt(sum);
        for(auto &i : window) i /= sum;
        windows.push_back(window);
    }

    int z = windows.size();
    vector<float> energy(z), req(z);
    vector<tuple<int, int, float, float> > shifted(z);

    for(int i=0; i<z; i++){
        auto [m, d, h] = wavelets[i];
        req[i] = d*44100.0/m;
    }

    for(int i=0; i<z; i++){
        
        double shift = 2.1;
        int j = 0;

        while(j+1 < z && req[j+1] < req[i]*shift) j++;

        int l = j;
        int r = j + 1 < z ? j + 1 : j;

        if(l == r){
            shifted[i] = {l, r, 0.0f, 1.0f};
        } else {
            float d = (req[i]*shift - req[l]) / (req[r] - req[l]);
            if(d < 0.0f) shifted[i] = {l, r, 0.0f, 1.0f};
            else shifted[i] = {l, r, 1.0f - d, d};
        }
    }

    /*
    for(auto [x, y, xx, yy] : shifted){
        std::cerr << x << ' ' << y << ' ' << xx << ' ' << yy << '\n';
    }
    */

    for(int i=0; i<n; i++){
        
        for(int f=0; f<z; f++){

            auto [m, d, h] = wavelets[f];
            auto &window = windows[f];

            if(i+m < n && i%(m/4) == 0){
                
                complex<float> sum = 0.0f;
                for(int j=0; j<m; j++) sum += audio[i+j] * window[j];
                
                sum *= h * 2;
                energy[f] = std::norm(sum);
                
                auto [ll, rr, dl, dr] = shifted[f];
                float e = energy[ll]*dl + energy[rr]*dr;
                if(std::abs(sum) != 0.0f){
                    sum = sum / std::abs(sum) * std::sqrt(e);
                }

                for(int j=0; j<m; j++) result[i+j] += (sum*std::conj(window[j])).real();
            
            }
        }
    }

    for(float &i : result) i *= 1.0 / 59.81;

    // if(rand()&1) return audio;
    return result;
}

}

