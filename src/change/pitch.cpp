#include "change/pitch.h"
#include "math/fft.h"
#include "math/ft.h"
#include "math/constants.h"
#include "ml/stack.h"
#include "change/detector.h"

#include <math.h>
#include <algorithm>
#include <iostream>

namespace change {

PeakMatchVars defaultPeakVars;
CorrelationVars defaultCorrelationVars;

float sign(float x){ return (int)(x > 0) - (x < 0); }

Detector _detector;

std::vector<float> peak_match_graph(const std::vector<float> &audio, PeakMatchVars vars){
    
    std::vector<float> food(128, 0.0f);
    for(int i=0; i<std::min(128, (int)audio.size()); i++) food[i] = audio[i];
    _detector.feed(food);
    
    return {(float)_detector.quiet*100, (float)_detector.voiced*100,
        _detector.confidence*100, _detector.pitch};

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

    std::vector<float> food(128, 0.0f);
    for(int i=0; i<std::min(128, (int)audio.size()); i++) food[i] = audio[i];
    _detector.feed(food);

    auto mse = _detector.get_mse();
    auto iavg = 1.0f / _detector.momentum.avg;
    for(float &i : mse) i *= iavg;

    return mse;
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

    float amax = 0.0f;
    for(float &i : clip) amax = std::max(amax, std::abs(i));
    if(amax != 0.0f) for(float &i : clip) i /= amax;

    int N = stack->in_size()-1;
    auto f = math::ft(audio, N/2);
    std::vector<float> freqs(N);
    for(int i=0; i<N/2; i++){
        freqs[2*i] = f[i].real();
        freqs[2*i+1] = f[i].imag();
    }
    
    freqs.push_back(44100 / (float)clip.size() * pitch);
    freqs = stack->run(freqs);

    for(int i=0; i<N/2; i++) f[i] = {freqs[2*i], freqs[2*i+1]};

    clip = math::ift(f, audio.size());

    float bmax = 0.0f;
    for(float &i : clip) bmax = std::max(bmax, std::abs(i));
    for(float &i : clip) i *= amax/bmax;

    return clip;
}

}
