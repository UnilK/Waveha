#include "change/pitch.h"
#include "math/fft.h"
#include "math/ft.h"
#include "math/sinc.h"
#include "math/constants.h"
#include "ml/stack.h"
#include "change/detector2.h"
#include "change/pitcher2.h"
#include "change/phaser2.h"
#include "change/phaser.h"
#include "change/util.h"
#include "change/tests.h"

#include <math.h>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <random>

namespace change {

PeakMatchVars defaultPeakVars;
CorrelationVars defaultCorrelationVars;

float sign(float x){ return (int)(x > 0) - (x < 0); }

std::vector<float> phase_graph(const std::vector<float> &audio, unsigned period){

    /*
    using std::vector;
    typedef std::complex<float> C;

    unsigned min = 6;
    unsigned x = 735;
    
    if(audio.size() <= x) return {0.0f, 0.0f};

    vector<unsigned> sizes;
    for(unsigned i=min; i<=x;){
        sizes.push_back(i);
        float j = i; i++;
        while(44100.0f/j - 44100/i < 1.0f) i++;
    }

    vector<float> all(audio.size()-x, 0.0f);

    for(unsigned i : sizes){
        
        vector<C > e(audio.size());

        for(unsigned j=0; j<audio.size(); j++){
            e[j] = math::cexp((float)j/i) * audio[j];
        }
        
        C sum = 0.0f;
        for(unsigned j=0; j<audio.size(); j++){
            sum += e[j];
            if(j>=i) sum -= e[j-i];
            if(j>=x) all[j-x] += (sum.real()*sum.real() + sum.imag()*sum.imag()) / (i*i);
        }
    }

    vector<float> norm(all.size(), 0.0f);

    float avg = 0.0f;
    for(unsigned i=0; i<all.size(); i++){
        avg += all[i];
        if(i >= 49) avg -= all[i-49];
        norm[i] = all[i] - avg / 49;
    }

    return norm;
    */

    if(audio.size() > 1000) return {0.0f, 0.0f};

    auto freq = math::ft(audio, (period+1)/2);
    /*
    std::vector<float> args(freq.size());
    for(unsigned i=0; i<freq.size(); i++) args[i] = std::arg(freq[i]);

    int n = args.size();

    for(int i=2; i<n; i++){
        float d = args[i-1]-args[i];
        d -= std::floor((d - M_PI) / (2 * M_PI)) * 2 * M_PI;
        args[i-1] += d * (i-1);
        args[i-1] -= std::floor(args[i-1]/(2*M_PI))*2*M_PI;
    }
    */
    
    std::vector<float> args(freq.size(), 0.0f);
    int n = args.size();

    /*
    for(int i=1; i<n; i++){
        auto c = freq[i]*std::conj(freq[i-1]);
        if(c != 0.0f) args[i] = std::arg(c);
    }
    */

    int d = std::max(1, (int)std::floor(450.0f / (44100.0/period)));

    for(int i=0; i<n; i++){
        std::complex<float> sum = 0.0f;
        for(int j=std::max(1, i-d); j<std::min(n, i+d+1); j++){
            sum += freq[j]*std::conj(freq[j-1]);
        }
        if(std::abs(sum) != 0.0f) args[i] = std::arg(sum);
    }

    return args;

    /*
    auto rot = [&](float d){   
        auto a = args;
        for(int i=0; i<N; i++) a[i] += (i+1)*d;
        for(auto &i : a) i -= std::floor(i/(2*PIF))*(2*PIF);
        for(int i=1; i<N; i++){
            while(std::abs(a[i]-a[i-1]) > std::abs(a[i]-a[i-1]+2*PIF)) a[i] += 2*PIF;
        }
        return a;
    };

    float pos = 0;
    float best = 1e9;
    for(int i=0; i<M; i++){
        auto a = rot((float)i*2*PIF/M);
        float sum = 0.0f;
        for(int i=1; i<N; i++) sum += abs(a[i]-a[i-1]);
        if(sum < best){
            best = sum;
            pos = (float)i*2*PIF/M;
        }
    }

    return rot(pos);
    */
}

std::vector<float> peak_match_graph(const std::vector<float> &audio, PeakMatchVars vars){

    using std::vector;
    using std::complex;
    using std::tuple;

    int n = audio.size();

    vector<float> windowed(n);
    // for(int i=0; i<n; i++) windowed[i] = audio[i] * (0.5f - 0.5f  * std::cos(2 * PIF * i / n));
    for(int i=0; i<n; i++) windowed[i] = audio[i] * std::sin(PIF * i / n);

    auto freq = math::fft(windowed);
    int m = n/2;

    vector<float> window(m, 0.0f);
    {
        double pw = 2.0;
        double reso = (44100.0 / n) / 1000.0 * pw;
        for(int i=0; i<m; i++) window[i] = std::exp(-i*reso) * std::pow(i*reso, pw);
    }

    vector<float> norm(m);
    for(int i=0; i<m; i++) norm[i] = std::abs(freq[i]) * window[i];

    return norm;
}

std::vector<float> correlation_graph(const std::vector<float> &audio, CorrelationVars vars){

    using std::vector;
    using std::complex;
    using std::tuple;

    int n = audio.size();

    /*
    vector<float> windowed(n);
    for(int i=0; i<n; i++) windowed[i] = audio[i] * std::sin(PIF * i / n);
 
    vector<float> window(n/2, 0.0f);
    {
        double pw = 2.0;
        double reso = (44100.0 / n) / 1000.0 * pw;
        for(int i=0; i<n/2; i++) window[i] = std::exp(-i*reso) * std::pow(i*reso, pw);
    }
    
    auto freq = math::fft(windowed);
    freq.resize(n/2);
    for(int i=0; i<n/2; i++) freq[i] = std::abs(freq[i]) * window[i];

    auto norm = math::inverse_fft(freq);
    norm.resize(n/4);

    float ma = 0.0f;
    for(int i=20; i+1<n/4; i++){
        if(norm[i] > norm[i-1] && norm[i] > norm[i+1]){
            ma = std::max(ma, norm[i]);
        }
    }

    std::cerr << ma / (norm[0] + 1e-9f) << '\n';

    ma = 1.0f / (norm[0] + 1e-9f);
    for(float &i : norm) i *= ma;

    return norm;
    */

    int m = n/2;

    int extra = 128;
    vector<float> a(m+extra), b(m+extra);
    for(int i=0; i<m+extra; i++){
        a[i] = audio[i] + rnd() * 1e-2;
        b[i] = audio[m+i-extra] + rnd() * 1e-2;
    }

    auto mse = math::emse(a, b);
    for(int i=0; i+2*extra < (int)mse.size(); i++) mse[i] = mse[i+2*extra];
    mse.resize(mse.size()-2*extra);
    for(float &i : mse) i = 1.0f - i;

    return mse;
}


std::vector<float> random_experiment(const std::vector<float> &audio){

    using std::vector;
    using std::complex;
    using std::tuple;

    /*
    Pitcher p(1.345, 16);

    std::vector<float> out;
    for(float i : audio){
        auto j = p.process(i);
        for(float k : j) out.push_back(k);
    }

    return out;
    */

    Phaser2 a(44100, 40.0f, 1000.0f);

    vector<float> result;
    for(float i : audio){
        a.push(i);
        result.push_back(a.pull());
        result.push_back(a.pull());
    }

    return result;

    // return energytranslate3(audio);
}

std::vector<float> random_experiment2(const std::vector<float> &audio, float low, float high){

    using std::vector;
    using std::complex;
    using std::tuple;

    int n = audio.size();

    vector<float> result(n, 0.0f), filtered(n, 0.0f);

    {
        const int N = 256;

        vector<complex<float> > cres(n, 0.0f);
        vector<float> window(N), fwindow(N);
        for(int i=0; i<N; i++) window[i] = (1.0f - std::cos(2 * PIF * i / N)) / sqrt(6);
        
        float reso = 44100.0f / N;
        
        low -= reso / 2;
        high += reso / 2;
        
        for(int i=0; i<=N-i; i++){
            float l = i * reso - reso / 2, r = i * reso + reso / 2;
            l = std::max(l, low);
            r = std::min(r, high);
            fwindow[i] = fwindow[(N-i)%N] = std::max(0.0f, (r-l)/reso);
        }

        for(int i=0; i+N<=n; i+=N/4){
            std::vector<float> bit(N);
            for(int j=0; j<N; j++) bit[j] = audio[i+j] * window[j];
            auto freq = math::fft(bit);
            for(int j=0; j<N; j++) freq[j] *= fwindow[j];
            
            for(int j=N/2; j<N; j++) freq[j] = 0.0f;
            math::in_place_fft(freq, 1);
            for(int j=0; j<N; j++) cres[i+j] += window[j] * freq[j];
            
            // bit = math::inverse_fft(freq);
            // for(int j=0; j<N; j++) result[i+j] += window[j] * bit[j];
        }

        for(int i=0; i<n; i++) result[i] = std::abs(cres[i]);
        for(int i=0; i<n; i++) filtered[i] = cres[i].real();
    }

    return filtered;
}

std::vector<std::complex<float> > candom_experiment(const std::vector<float> &audio){
    
    using std::vector;
    using std::complex;
    using std::tuple;
    
    int n = audio.size();
    int N = 128;

    vector<float> window(N);
    for(int i=0; i<N; i++) window[i] = 0.5f - 0.5f * std::cos(2 * PIF * i / N);

    vector<float> waudio(n, 0.0f);
    for(int i=0; i+N<=n; i+=N/4){
        vector<float> bit(N);
        for(int j=0; j<N; j++) bit[j] = audio[i+j] * window[j];
        auto freq = math::fft(bit);
        int d = N / 24;
        freq[0] = 0.0f;
        for(int j=1; j<=d; j++) freq[j] = freq[N-j] = 0.0f;
        bit = math::inverse_fft(freq);
        for(int j=0; j<N; j++) waudio[i+j] += window[j] * bit[j];
    }

    auto vivo = waudio;
    for(float &i : waudio) i = 0.0f;

    for(int i=0; i+N<=n; i+=N/4){
        vector<float> bit(N);
        for(int j=0; j<N; j++) bit[j] = std::abs(vivo[i+j]) * window[j];
        auto freq = math::fft(bit);
        int d = N / 32;
        for(int j=d+1; j<N-d; j++) freq[j] = 0.0f;
        bit = math::inverse_fft(freq);
        for(int j=0; j<N; j++) waudio[i+j] += window[j] * bit[j];
    }

    vector<complex<float> > freq(n);
    for(int i=0; i<n; i++) freq[i] = waudio[i];

    return freq;
}

std::vector<float> ml_graph(ml::Stack *stack, const std::vector<float> &audio, float pitch){

    if(stack == nullptr || !stack->good()) return std::vector<float>(audio.size(), 3.14f);
    
    std::vector<float> clip = audio;

    int N = stack->in_size()-2;
    auto f = math::ft(audio, N/2);
    
    float sum = 0;
    for(auto i : f) sum += (i * std::conj(i)).real();
    sum = std::sqrt(sum);

    if(sum != 0.0f){
        float isum = 1.0f / sum;
        for(auto &i : f) i *= isum;
    }

    std::vector<float> freqs(N);
    for(int i=0; i<N/2; i++){
        freqs[2*i] = f[i].real();
        freqs[2*i+1] = f[i].imag();
    }
    
    freqs.push_back(pitch);
    freqs.push_back(1.0f);
    freqs = stack->run(freqs);

    for(int i=0; i<N/2; i++) f[i] = {freqs[2*i], freqs[2*i+1]};

    float sum2 = 0.0f;
    for(float i : freqs) sum2 += i*i;
    sum2 = std::sqrt(sum2);

    if(sum2 != 0.0f){
        float mul = sum / sum2;
        for(auto &i : f) i *= mul;
    }

    clip = math::ift(f, audio.size());

    return clip;
}

std::vector<float> pitch_changer(std::vector<float> audio, float pitch){
    
    unsigned size = audio.size();
    unsigned nsize = audio.size() / pitch;

    const int F = 64;

    std::vector<float> mag(F, 0.0f);
    for(unsigned i=0; i<size; i++){
        std::vector<float> time(2*nsize);
        for(unsigned j=0; j<2*nsize; j++) time[j] = audio[(j+i)%size];
        auto x = math::cos_window_ft(time, F);
        for(int j=0; j<F; j++) mag[j] += std::abs(x[j]);
    }

    auto freq = math::ft(audio, F);
    
    for(auto &i : freq){
        float d = std::abs(i);
        if(i != 0.0f) i /= d;
    }

    for(unsigned j=0; j<F; j++) freq[j] *= mag[j] / (float)size;

    return math::ift(freq, nsize, 0);
}

std::vector<float> pitch_translate(float pitch){
   
    float top = 5.0f;
    float step = top / 50;

    std::vector<float> graph;

    for(float x=top; x>=0.0f; x-=step){

        int n = 1<<12;
        int m = n * x;

        std::vector<float> w(n+2*m);
        for(int i=0; i<n+2*m; i++) w[i] = math::cexp((double)i/n).real();

        float avg = 0.0f;

        for(int i=0; i<n; i++){
            std::vector<float> v(2*m);
            for(int j=0; j<2*m; j++) v[j] = w[i+j];
            
            std::complex<float> c = 0.0f;
            for(int j=0; j<2*m; j++) v[j] *= 0.5f - 0.5f * math::cexp((double)j/(2*m)).real();
            for(int j=0; j<2*m; j++) c += v[j]*math::cexp(-(double)j/m);

            avg += 2 * abs(c) / n;
        }

        avg /= n;

        graph.push_back(avg);
    }

    return graph;
}

std::vector<std::complex<float> > translate(
        const std::vector<std::complex<float> > &f,
        float pitch){
    
    using std::vector;
    using std::complex;

    int n = f.size();
    int m = std::max(1, (int)std::floor(pitch * n));

    vector<float> leni(n, 0.0f);
    vector<float> leno(m, 0.0f);

    for(int i=0; i<n; i++) leni[i] = std::norm(f[i]);

    vector<complex<float> > argi(n, 1.0f);
    vector<complex<float> > argo(m, 0.0f);

    for(int i=1; i<n; i++){
        argi[i] = f[i] * std::conj(f[i-1]);
    } if(n > 1) argi[0] = argi[1];

    {
        auto tmp = argi;
        for(int i=0; i<n; i++){
            argi[i] = 0.0f;
            int cnt = 0;
            for(int j=-4; j<=4; j++){
                if(i+j >= 0 && i+j < n){
                    argi[i] += tmp[i+j];
                    cnt++;
                }
            }
            argi[i] /= cnt;
        }
    }

    if(pitch >= 1.0f){

        for(int i=0, j=0; i<n; i++){
            if((j+1)*pitch < i+1) j++;
            if(j == 0){
                leno[j] += leni[i];
                argo[j] += argi[i];
            } else if(j<m){
                float d = ((j+1)*pitch - (i+1)) / pitch;
                leno[j-1] += leni[i] * d;
                leno[j] += leni[i] * (1.0f - d);
                argo[j-1] += argi[i] * d;
                argo[j] += argi[i] * (1.0f - d);
            }
        }

    } else {

        vector<float> lens(n, 0.0f);
        
        for(int i=0, j=0; j<m; j++){
            if((j+1)*pitch > i+1) i++;
            if(i == 0) lens[i] += 1;
            else if(i<n){
                float d = (i+1) - (j+1)*pitch;
                lens[i-1] += d;
                lens[i] += (1.0f - d);
            }
        }
        
        for(int i=0; i<n; i++){
            if(lens[i] != 0.0f){
                leni[i] /= lens[i];
                argi[i] /= lens[i];
            }
        }

        for(int i=0, j=0; i<n; j++){
            if((j+1)*pitch > i+1) i++;
            if(i == 0){
                leno[j] = leni[i];
                argo[j] = argi[i];
            }
            else if(j<m){
                float d = (i+1) - (j+1)*pitch;
                leno[j] = leni[i] * (1.0f - d) + leni[i-1] * d;
                argo[j] = argi[i] * (1.0f - d) + argi[i-1] * d;
            }
        }

    }

    for(int i=0; i<m; i++){
        float d = std::abs(argo[i]);
        if(d == 0.0f) argo[i] = 1.0f;
        else argo[i] /= d;
        argo[i] = std::pow(argo[i], -i-1);
        d = std::abs(argo[i]);
        if(d == 0.0f) argo[i] = 1.0f;
        else argo[i] /= d;
    }

    for(int i=0; i<m; i++) argo[i] *= std::sqrt(leno[i]);

    return argo;
}

}
