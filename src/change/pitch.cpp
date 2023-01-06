#include "change/pitch.h"
#include "math/fft.h"
#include "math/ft.h"
#include "math/sinc.h"
#include "math/constants.h"
#include "ml/stack.h"
#include "change/detector2.h"
#include "change/changer1.h"
#include "change/changer2.h"
#include "change/changer3.h"
#include "change/pitcher.h"
#include "change/pitcher2.h"
#include "change/phaser2.h"
#include "change/resample.h"
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

Phaser2 _phaser(44100, 80.0f, 300.0f);
Changer2 _changer;

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
   
    return {0.0f, 0.0f};

    /*
    auto plot = audio;
    auto freq = math::fft(plot);
    auto nfreq = freq;
    for(auto &i : nfreq) i = 0.0f;

    unsigned low = 60 * audio.size() / 44100;
    unsigned high = 1000 * audio.size() / 44100;

    unsigned n = freq.size();
    for(unsigned i=low; i<=high; i++){
        nfreq[i] = freq[i];
        nfreq[(n-i)%n] = std::conj(freq[i]);
    }
    
    plot = math::inverse_fft(nfreq);
    */

    /*
    std::vector<float> plot(audio.size(), 0.0f);
    for(unsigned i=1; i+1<audio.size(); i++){
        plot[i] = std::abs(audio[i-1]+audio[i+1]-2*audio[i]) + plot[i-1];
    }

    float sum = 0.0f, avg = 0.0f;
    for(float i : audio) avg += i;
    avg /= audio.size();

    for(unsigned i=1; i+1<audio.size(); i++){
        sum += (audio[i]-avg)*(audio[i]-avg);
        if(sum != 0.0f) plot[i] /= sum;
    }
    */

    /*
    auto plot = audio;

    for(float &i : plot) i = i*i*i;

    float max = 0.0f;
    for(float i : plot) max = std::max(max, std::abs(i));
    if(max != 0.0f) for(float &i : plot) i /= max;
    */

    /*
    auto plot = audio;
    
    for(float &i : plot) i = i*i*i*i;

    auto freq = math::fft(plot);
    unsigned f = 1000 * audio.size() / 44100;
    for(unsigned i=f; i+f<=freq.size(); i++) freq[i] = 0.0f;
    plot = math::inverse_fft(freq);
    
    for(float &i : plot) i = sign(i)*std::pow(std::abs(i), 0.5f);

    float sum = 0.0f;
    for(float i : audio) sum += i*i;
    sum /= audio.size();

    if(sum != 0.0f) for(float &i : plot) i /= sum;
    */

    /*
    auto spectrum = math::fft(audio);

    std::vector<float> plot(audio.size(), 0.0f);

    for(unsigned i = 2 * vars.peaks; i * 2 < plot.size(); i++){
        
        float fundamental = (float)plot.size() / i;

        for(int j = 1; j <= vars.peaks; j++){
            
            float frequency = fundamental * j;
            int k = std::floor(frequency);
            float d  = frequency - k;

            if(k+1 > (int)audio.size() / 2) break;

            // there's a phase flip on the peak so use - instead of +.
            plot[i] += std::pow(std::abs(spectrum[k] * (1 - d) - spectrum[k+1] * d), vars.exponent);

        }
    }
    */

    // return plot;

}

std::vector<float> random_experiment(const std::vector<float> &audiox){

    /*
    Pitcher2 p(1.0, 32);

    std::vector<float> out;
    for(float i : audio){
        auto j = p.process(i);
        for(float k : j) out.push_back(k);
    }
    */

    /*
    std::vector<float> audio = audiox;

    int n = audio.size();
    std::vector<float> out(n);

    for(float &i : audio) i = 2 * rnd() - 1;

    if(rand()&1){
        std::vector<std::vector<std::complex<float> > > w;
        for(int i=1; 44100.0f/(4*i)>90.0f; i++){
            int m = i * 4;
            std::vector<std::complex<float> > v(m);
            for(int j=0; j<m; j++){
                v[j] = (1.0 - std::cos(2 * PI * j / m)) / std::sqrt(6*m) / 3;
                v[j] *= std::polar(1.0, 4 * PI * j / m);
            }
            w.push_back(v);
        }

        w.push_back(w.back());
        for(auto &i : w.back()) i = std::abs(i);

        std::cerr << w.size() << '\n';

        for(int i=0; i<n; i++){
            for(auto v : w){
                int m = v.size();
                if(i%(m/4) == 0 && i+m < n){
                    std::complex<float> sum = 0.0f;
                    for(int j=0; j<m; j++) sum += audio[i+j] * v[j];
                    for(int j=0; j<m; j++) out[i+j] += (sum * std::conj(v[j])).real();
                }
            }
        }
    } else out = audio;

    for(int i=0; i<n; i++) out[i] *= (1.0f - std::cos(2*PI*i/n)) * 0.5f;
    */

    /*
    int n = out.size();
    auto filtered = out;
    for(float &i : out) i = 0.0f;

    {
        const int N = 32, M = 4;

        std::vector<float> window(N), fwindow(N, 0.0f);
        for(int i=0; i<N; i++) window[i] = (std::cos(2*PI*i/N) - 1.0) / M * 4 / sqrt(3);

        float reso = 44100.0f / N;
        float low = -reso / 2, high = 8000.0f;
        for(int i=0; i<=N-i; i++){
            float l = i * reso - reso / 2, r = i * reso + reso / 2;
            
            l = std::max(l, low);
            r = std::min(r, high);

            fwindow[i] = fwindow[(N-i)%N] = std::max(0.0f, (r-l)/reso);
        }

        for(int i=0; i+N<=n; i+=N/M){
            std::vector<float> bit(N);
            for(int j=0; j<N; j++) bit[j] = filtered[i+j] * window[j];
            auto freq = math::fft(bit);
            for(int j=0; j<N; j++) freq[j] *= fwindow[j];
            bit = math::inverse_fft(freq);
            for(int j=0; j<N; j++) out[i+j] += window[j] * bit[j];
        }
    }

    for(int i=0; i<n; i++) out[i] *= (1.0f - std::cos(2*PI*i/n)) * 0.5f;
    */

    /*
    int n = audio.size();
    std::vector<float> out(n, 0.0f);

    {
        const int N = 256, M = 4;

        std::vector<float> window(N), fwindow(N, 0.0f);
        for(int i=0; i<N; i++) window[i] = (std::cos(2*PI*i/N) - 1.0) / M * 4 / sqrt(3);

        float reso = 44100.0f / N;
        float low = 300.0f, high = 1500.0f;
        for(int i=0; i<=N-i; i++){
            float l = i * reso - reso / 2, r = i * reso + reso / 2;
            
            l = std::max(l, low);
            r = std::min(r, high);

            fwindow[i] = fwindow[(N-i)%N] = std::max(0.0f, (r-l)/reso);
        }

        for(int i=0; i+N<=n; i+=N/M){
            std::vector<float> bit(N);
            for(int j=0; j<N; j++) bit[j] = audio[i+j] * window[j];
            auto freq = math::fft(bit);
            for(int j=0; j<N; j++) freq[j] *= fwindow[j];
            bit = math::inverse_fft(freq);
            for(int j=0; j<N; j++) out[i+j] += window[j] * bit[j];
        }
    }
    
    auto filtered = out;
    for(float &i : out) i = 0.0f;

    if(rand()&1){

        const int N = 256, M = 8;

        std::vector<float> window(N);
        for(int i=0; i<N; i++) window[i] = (std::cos(2*PI*i/N) - 1.0) / M * 4 / sqrt(3);

        for(int i=0; i+N<=n; i+=N/M){
            
            std::vector<float> bit(N);
            for(int j=0; j<N; j++) bit[j] = filtered[i+j] * window[j];
            
            auto freq = math::fft(bit);

            std::vector<float> a(N, 0.0f), b(N, 0.0f);
            for(int j=0; j<N; j++) a[j] = std::norm(freq[j]);

            for(int j=0; j<=N-j; j++){
                if(j&1){
                    b[j/2] += a[j]/2;
                    b[j/2+1] += a[j]/2;
                } else {
                    b[j/2] += a[j];
                }
            }

            for(int j=0; j<N; j++) if(std::abs(freq[j]) != 0.0f) freq[j] /= std::abs(freq[j]);
            for(int j=0; j<N; j++) freq[j] *= std::sqrt(b[j]);
            for(int j=1; j<N-j; j++) freq[N-j] = std::conj(freq[j]);

            bit = math::inverse_fft(freq);
            for(int j=0; j<N; j++) out[i+j] += window[j] * bit[j];
        }
    } else {
        out = filtered;
    }
    */
   
    // return out;
    return hodgefilter(audiox);
}

std::vector<std::complex<float> > candom_experiment(const std::vector<float> &audio){

    auto filtered = random_experiment(audio);

    int n = audio.size()/2;
    const int N = 32;
    
    std::vector<float> bit(N), window(N);
    for(int i=0; i<N; i++) window[i] = (std::cos(2*PI*i/N) - 1.0) * 0.5;

    for(int i=0; i<N; i++) bit[i] = filtered[n+i] * window[i];

    auto out = math::fft(bit);
    out.resize(N/2);

    return out;
}

std::vector<float> correlation_graph(const std::vector<float> &audio, CorrelationVars vars){

    /*
    auto filter = [](std::vector<float> &plot) -> void {
        for(float &i : plot) i *= i;
        plot = math::ift(math::ft(plot, 20), plot.size());
    };
    */

    /*
    unsigned n = audio.size();
    std::vector<float> corr(audio.size(), 0.0f);

    for(unsigned i=1; 2*i<=audio.size(); i++){
        for(unsigned j=0; j<i; j++){
            float d = audio[j] - audio[i+j];
            corr[i] += d*d;
        }
        corr[i] /= i;
    }
    
    float sum = 0.0f;
    for(float i : audio) sum += i*i;
    sum /= n;

    if(sum != 0.0f) for(float &i : corr) i /= sum;
    */
   
    /*
    unsigned n = 1470 / 2;
    std::vector<float> left(n), right(n);
    for(unsigned i=0; i<std::min((unsigned)audio.size(), n); i++){
        left[i] = audio[i];
        right[i] = audio[n+i];
    }

    // filter(left);
    // filter(right);

    auto corr = math::correlation(left, right);

    float sum = 0.0f;
    for(float i : left) sum += i*i;
    for(float i : right) sum += i*i;

    for(unsigned i=0; i<n; i++){
        corr[i] = (sum - 2 * corr[i]) / (n-i);
        sum -= left[i]*left[i] + right[n-i-1]*right[n-i-1];
    }

    corr.resize(n+1);
    reverse(corr.begin(), corr.end());

    sum = 0.0f;
    for(unsigned i=1; i<=n; i++){
        corr[i] = abs(corr[i]);
        sum += corr[i];
        if(sum/i != 0.0f) corr[i] /= (sum/i);
    }

    return corr;
    */
  
    /*
    unsigned n = 1470;
    std::vector<float> plot(n, 0.0f);
    for(int i=(int)audio.size()-1, j=n-1; i>=0 && j>=0; i--, j--) plot[j] = audio[i];

    auto [corr, tmp] = math::autocorrelation(plot);

    float sum = 0.0f;
    for(float i : plot) sum += i*i;
    sum *= 2;

    corr[0] = 0.0f;
    for(unsigned i=1; i<n; i++){
        sum -= plot[i-1]*plot[i-1] + plot[n-i]*plot[n-i];
        corr[i] = (sum - 2 * corr[i]) / (n - i);
    }

    sum = 0.0f;
    for(unsigned i=1; i<n; i++){
        sum += corr[i];
        if(sum != 0.0f) corr[i] /= (sum/i);
    }
    */

    /*
    if(vars.sign) for(float &i : copy) i = sign(i) * std::pow(std::abs(i), vars.exponent);
    else for(float &i : copy) i = std::pow(std::abs(i), vars.exponent);

    auto reverseCopy = copy;
    std::reverse(reverseCopy.begin(), reverseCopy.end());

    auto plot = math::convolution(copy, reverseCopy);
    */

    /*
    static std::vector<float> previous;

    n /= 2;

    previous.resize(n, 0.0f);
    auto cumu = corr;
    cumu.resize(n);

    for(unsigned i=0; i<n; i++) cumu[i] += previous[i] * 0.9;
    
    float avg = 0.0f, min = 1e9;
    for(unsigned i=60; i<n; i++){
        avg += std::abs(cumu[i]);
        min = std::min(min, std::abs(cumu[i]));
    }

    avg /= n;
    if(avg != 0.0f) min /= avg;

    for(unsigned i=0; i<n; i++) previous[i] = previous[i] * 0.7f + (1.0f - min) * corr[i];
    */

    /*
    std::vector<float> food(128, 0.0f);
    for(int i=0; i<std::min(128, (int)audio.size()); i++) food[i] = audio[i];
    _detector.feed(food);

    auto mse = _detector.get_mse();
    
    return mse;
    */

    /*
    int low = 128;
    auto f = math::fft(audio);
    for(auto &i : f) i = std::abs(i);
    for(unsigned i=f.size()/2; i<f.size(); i++) f[i] = 0.0f;
    math::in_place_fft(f);
    for(int i=-low; i<=low; i++) f[std::abs((int)f.size()+i)%f.size()] *= 2*(float)(low - std::abs(i)) / low;
    for(unsigned i=low+1; i+low<f.size(); i++) f[i] = 0.0f;
    math::in_place_fft(f, 1);
    std::vector<float> a(f.size());
    for(unsigned i=0; i<f.size(); i++) a[i] = f[i].real();
    a.resize(a.size()/2);
    */

    /*
    for(float i : audio) _detector.push(i);
    std::cerr << _detector.pitch() << '\n';

    return _detector.debug();
    */
    
    for(float i : audio){
        _phaser.push(i);
        _phaser.pull();
    }

    return _phaser.debug();
}

unsigned pitch(const std::vector<float> &audio, CorrelationVars vars){

    auto copy = audio;

    if(vars.sign) for(float &i : copy) i = sign(i) * std::pow(std::abs(i), vars.exponent);
    else for(float &i : copy) i = std::pow(std::abs(i), vars.exponent);

    auto reverseCopy = copy;
    std::reverse(reverseCopy.begin() + 1, reverseCopy.end());

    auto plot = math::convolution(copy, reverseCopy);

    unsigned max = 50;
    float m = 0.0f;
    unsigned n = audio.size()-1;
    
    for(unsigned i=50; n+i<plot.size() && i < 800; i++){
        if(m < plot[n+i]){
            m = plot[n+i];
            max = i;
        }
    }

    return max;
}

unsigned pitch(const float *audio, unsigned size, CorrelationVars vars){
    std::vector<float> aa(size);
    for(unsigned i=0; i<size; i++) aa[i] = audio[i];
    return pitch(aa, vars);
}

unsigned hinted_pitch(const std::vector<float> &audio, unsigned hint, CorrelationVars vars){
    return hinted_pitch(audio.data(), audio.size(), hint, vars);
}

unsigned hinted_pitch(const float *audio, unsigned size, unsigned hint, CorrelationVars vars){
    
    if(size < 2 * hint) return hint;

    std::vector<float> left(hint), right(hint);
    for(unsigned i=0; i<hint; i++) left[i] = audio[i];
    for(unsigned i=0; i<hint; i++) right[i] = audio[hint+i];
    
    if(vars.sign){
        for(float &i : left) i = sign(i) * std::pow(std::abs(i), vars.exponent);
        for(float &i : right) i = sign(i) * std::pow(std::abs(i), vars.exponent);
    }
    else {
        for(float &i : left) i = std::pow(std::abs(i), vars.exponent);
        for(float &i : right) i = std::pow(std::abs(i), vars.exponent);
    }

    std::vector<float> c = math::correlation(left, right);

    float biggest = c[0];
    unsigned offset = 0;

    unsigned csize = c.size();

    for(unsigned i=1; i<hint; i++){

        if(c[i] > biggest){
            biggest = c[i];
            offset = i;
        }
        
        if(c[csize-i] > biggest){
            biggest = c[csize-i];
            offset = -i;
        }
    }

    return hint + offset;
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
