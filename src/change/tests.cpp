#include "change/tests.h"
#include "change/pitcher2.h"
#include "change/phaser.h"
#include "change/phaser2.h"
#include "change/phaser4.h"
#include "math/fft.h"
#include "math/constants.h"
#include "change/util.h"
#include "change/pitch.h"
#include "designa/math.h"
#include "designh/knot.h"
#include "designi/knot.h"
#include "designg/wavelet.h"
#include "designg/common.h"

#include <complex>
#include <random>
#include <iostream>
#include <cassert>
#include <tuple>
#include <deque>

namespace change {

std::vector<float> translate(const std::vector<float> &audio){
   
    using std::vector;
    using std::complex;

    std::vector<float> result;
    designi::Knot knot(44100, 60.0f, 512);

    int esize = knot.get_eq_frequency_window_size();

    vector<float> gain(esize, 1.0f);
    vector<float> clean(esize, 9e-5f);

    knot.set_eq_gain(gain);
    knot.set_eq_clean(clean);
    
    knot.enable_eq_gain(0);
    knot.enable_eq_clean(0);
   
    knot.set_bind_threshold(0.8f);

    knot.set_absolute_pitch_shift(1.7f);
    knot.set_color_shift(1.3f);

    knot.enable_equalizer(0);
    knot.enable_binder(0);

    std::cerr << knot.get_delay() << '\n';

    for(float i : audio) result.push_back(knot.process(i));

    return result;
}

std::vector<float> pitchenvelope(const std::vector<float> &audio){

    using std::vector;
    using std::complex;
    using std::tuple;

    int N = audio.size();
    const int n = 1<<11, m = 1<<10, k = 1<<9;
    float reso = 44100.0 / n;
    int min = std::floor(900.0f/reso/2);

    vector<float> enve, window(n), fwindow(m), norm(m), windowed(n);
    
    for(int i=0; i<n; i++) window[i] = std::sin(PIF * i / n);
   
    {
        int wlow = std::ceil(2000.0 / (44100.0 / n));
        int wlen = std::ceil(200.0 / (44100.0 / n));
        for(int i=0; i<m; i++){
            if(i>wlow+wlen) fwindow[i] = 0.0f;
            else if(i<wlow-wlen) fwindow[i] = 1.0f;
            else fwindow[i] = (float)(wlow-i+wlen)/(2*wlen+1);
        }
    }

    auto para1 = [](float i, float l, float m, float r) -> tuple<float, float> {
        const float a = 0.5f * (r+l) - m;
        const float b = (r-l) * 0.5f;
        const float d = - b / (2 * a);
        return {m + b*d + a*d*d, i+d};
    };
    
    auto para2 = [](float i, float l, float m, float r) -> float {
        const float a = 0.5f * (r+l) - m;
        const float b = (r-l) * 0.5f;
        return m + b*i + a*i*i;
    };

    for(int x=0; x+n<=N; x+=n/16){

        for(int i=0; i<n; i++) windowed[i] = audio[x+i] * window[i];
        auto freq = math::fft(windowed);
        for(int i=0; i<m; i++) freq[i] = std::abs(freq[i]) * fwindow[i];
        freq.resize(m);
        norm = math::inverse_fft(freq);
    
        vector<tuple<float, float> > peaks(8, {0.0f, 22050.0f});
        auto add = [&](tuple<float, float> peak) -> void {
            for(auto &i : peaks) if(i < peak) swap(i, peak);
        };

        for(int i=min; i<k; i++){
            if(norm[i] > norm[i-1] && norm[i] > norm[i+1]){
                add(para1(i, norm[i-1], norm[i], norm[i+1]));
            }
        }

        for(auto &[a, f] : peaks){
            float aa = a;
            for(int i=2; i*f < k/2; i++){
                int j = std::round(i*f);
                a = std::min(a, i*para2(i*f-j, norm[j-1], norm[j], norm[j+1]));
            }
            a = std::max(aa * 0.5f, a);
        }

        tuple<float, float> best = {0.0f, 0.0f};
        for(auto i : peaks) if(i > best) best = i;

        auto [amp, pitch] = best;
        enve.push_back(44100.0 / (2 * pitch));
    }

    return enve;
}

std::vector<float> frequencywindow(){
    
    /*
    using std::vector;
    using std::tuple;
    using std::complex;

    static unsigned f = 0;
    vector<vector<complex<float> > > windows;
    vector<tuple<int, float> > params;

    const double F = 44100.0;
    {
        double len = 160.0;
        double freq = 0;
        double fmul = 1.1;

        while(freq < 6000.0f){
            
            int m = std::round((2*F/len)/4)*4;
            vector<complex<float> > window(m);
            params.push_back({m, freq});

            double turn = 2 * PI * freq / F;
            for(int i=0; i<m; i++){
                float len = 0.5f-0.5f*std::cos(2*PIF*i/m);
                window[i] = std::polar<float>(len, -turn*i);
            }

            freq += len/4;
            len *= fmul;

            windows.push_back(window);
        }

        for(auto &i : windows[0]) i /= sqrt(2);
    }

    const int n = 1<<12;
    const int N = 1<<10;

    static vector<vector<float> > sums;
    sums.resize(windows.size());
    for(auto &i : sums) i.resize(n, 0.0f);
    
    vector<float> enve(n, 0.0f), audio(n);

    if(f == windows.size()){
        for(auto i : sums){
            for(int j=0; j<n; j++) enve[j] += i[j];
        }
    } else {
        
        auto &window = windows[f];
        auto [m, ff] = params[f];

        for(int x=0; x<N; x++){
            for(float &i : audio) i = 2.0 * rnd() - 1.0;
            std::vector<float> result(n, 0.0f);
            for(int i=0; i+m<n; i+=m/4){
                complex<float> sum = 0.0f;
                for(int j=0; j<m; j++) sum += audio[i+j] * window[j];
                sum *= 2.0/m;
                for(int j=0; j<m; j++) result[i+j] += (sum*std::conj(window[j])).real();
            }
            for(int i=0; i<n; i++) result[i] *= (1.0f - std::cos(2 * PIF * i / n)) * 0.5f;
            for(int i=0; i<n; i++) result[i] /= 6;
            auto freq = math::fft(result);
            for(int i=0; i<n; i++) enve[i] += std::abs(freq[i]);
        }

        for(float &i : enve) i /= N;

        sums[f] = enve;
    }

    f = (f+1)%(windows.size()+1);
    
    return enve;
    */

    return {0.0f, 0.0f};
}

std::vector<float> energytranslate(const std::vector<float> &audio){

    /*
    using std::vector;
    using std::tuple;
    using std::complex;

    static int ff = 0;

    int n = audio.size();
    const int N = 256;
    const int H = N/2 + 1;

    vector<float> output(n, 0.0f);
    vector<vector<float> > iband(H, vector<float>(n, 0.0f));
    vector<vector<float> > oband(H, vector<float>(n, 0.0f));

    {
        vector<float> window(N);
        for(int i=0; i<N; i++) window[i] = (1.0f - std::cos(2 * PIF * i / N)) / sqrt(6);

        vector<vector<complex<float> > > waves(H, vector<complex<float> >(N));
        for(int i=0; i<H; i++){
            for(int j=0; j<N; j++) waves[i][j] = std::polar(window[j] / N, 2 * PIF * j * i / N);
        }
        for(auto &i : waves[0]) i /= 2;
        for(auto &i : waves[H-1]) i /= 2;

        for(int i=0; i+N<n; i+=N/4){
            vector<float> bit(N);
            for(int j=0; j<N; j++) bit[j] = audio[i+j] * window[j];
            auto freq = math::fft(bit);
            for(int h=0; h<H; h++){
                for(int j=0; j<N; j++) iband[h][i+j] += 2 * (waves[h][j] * freq[h]).real();
            }
        }
    }

    const float shift = 0.4;
    const float reso = 44100.0 / N;

    vector<float> f(H);
    vector<int> length(H);
    vector<vector<float> > window(H);

    for(int h=0; h<H; h++) f[h] = reso * h;

    for(int h=1; h<H; h++){
        length[h] = std::ceil(N / sqrt(h));
        length[h] = (length[h] + 3) / 4 * 4;
    } length[0] = length[1];

    for(int h=0; h<H; h++){
        window[h].resize(length[h]);
        for(int i=0; i<length[h]; i++){
            window[h][i] = (1.0f - std::cos(2 * PIF * i / length[h])) / 4;
        }
    }
    
    auto genshift = [&](vector<float> fre, vector<float> shift){

        vector<vector<tuple<int, float> > > g(H);

        for(int i=0; i<H; i++){

            int j=0;
            while(j+1 < H && fre[j+1] < fre[i]*shift[i]) j++;
            
            if(j+1 == H) continue;
            
            float d = (fre[i]*shift[i]-fre[j]) / (fre[j+1] - fre[j]);

            g[j].push_back({i, 1.0f - d});
            g[j+1].push_back({i, d});
        }

        return g;
    };

    vector<vector<tuple<int, float> > > g;
    {
        vector<float> fre, shi(H, shift);
        for(int i=0; i<H; i++) fre.push_back(i*reso);
        shi[0] = 1.0f;
        shi[1] = 0.75f + 0.25f * shi[1];
        shi[2] = 0.50f + 0.50f * shi[2];
        shi[3] = 0.25f + 0.75f * shi[3];
        g = genshift(fre, shi);
    }

    for(int h=0; h<H; h++){
        for(int i=0; i+length[h]<n; i+=length[h]/4){
            
            float isum = 0.0f, osum = 0.0f;
            
            for(int j=0; j<length[h]; j++){
                const float s = window[h][j] * iband[h][i+j];
                isum += s*s;
            }
            
            for(auto [k, d] : g[h]){
                float sum = 0.0f;
                for(int j=0; j<length[h]; j++){
                    const float s = window[h][j] * iband[k][i+j];
                    sum += s*s;
                }
                osum += d * sum;
            }
            
            float coef = isum == 0.0f ? 0.0f : std::sqrt(osum / isum);

            for(int j=0; j<length[h]; j++){
                oband[h][i+j] += window[h][j] * iband[h][i+j] * coef;
            }
        }
    }
    
    for(int h=0; h<H; h++){
        for(int i=0; i<n; i++) output[i] += oband[h][i];
    }

    ff = (ff+1)%(window.size());
    return output;
    */

    return audio;
}

std::vector<float> energytranslate2(const std::vector<float> &audio){

    /*
    using std::vector;
    using std::tuple;
    using std::complex;

    int n = audio.size();

    vector<vector<complex<float> > > windows;
    vector<tuple<int, float, float> > params;

    const double F = 44100.0;
    {
        double len = 160.0;
        double freq = 0;
        double fmul = 1.1;

        while(freq < 6000.0f){
            
            int m = std::round((2*F/len)/4)*4;
            vector<complex<float> > window(m);

            double turn = 2 * PI * freq / F;
            for(int i=0; i<m; i++){
                float len = 0.5f-0.5f*std::cos(2*PIF*i/m);
                window[i] = std::polar<float>(len, -turn*i);
            }

            params.push_back({m, freq, (sqrt(6)/8)});
            windows.push_back(window);
            
            freq += len * 0.25f;
            len *= fmul;
        }

        { auto &[xx, yy, zz] = params[0]; zz *= 0.5f; }

    }

    int z = windows.size();

    auto calcband = [&](int w) -> vector<complex<float> > {
       
        vector<complex<float> > out(n, 0.0f);
        
        auto &window = windows[w];
        auto [m, fre, mul] = params[w];

        for(int i=0; i+m<=n; i+=m/4){
            complex<float> sum = 0.0f;
            for(int j=0; j<m; j++) sum += conj(window[j])*audio[i+j];
            sum *= mul * 2 / m;
            for(int j=0; j<m; j++) out[i+j] += window[j]*sum;
        }


        return out;
    };
    
    auto cnorm = [&](const vector<complex<float> > &v) -> vector<float > {
        vector<float> nor(n);
        for(int i=0; i<n; i++) nor[i] = std::norm(v[i]);
        return nor;
    };

    auto genshift = [&](vector<float> fre, vector<float> shift){

        vector<vector<tuple<int, float> > > g(z);

        for(int i=0; i<z; i++){

            int j=0;
            while(j+1 < z && fre[j+1] < fre[i]*shift[i]) j++;
            
            if(j+1 == z) continue;
            
            float d = (fre[i]*shift[i]-fre[j]) / (fre[j+1] - fre[j]);

            g[j].push_back({i, 1.0f - d});
            g[j+1].push_back({i, d});
        }

        return g;
    };

    vector<vector<tuple<int, float> > > g;
    {
        float s = 0.4f;
        vector<float> fre, shift(z, s);
        for(auto [xx, yy, zz] : params) fre.push_back(yy);
        shift[0] = 1.0f;
        shift[1] = 0.75f + 0.25f * shift[1];
        shift[2] = 0.50f + 0.50f * shift[2];
        shift[3] = 0.25f + 0.75f * shift[3];
        g = genshift(fre, shift);
    }

    vector<vector<complex<float> > > iband(z);
    vector<vector<float> > norms, oband(z, vector<float>(n, 0.0f));

    for(int i=0; i<z; i++) iband[i] = calcband(i);
    for(auto &i : iband) norms.push_back(cnorm(i));

    for(int h=0; h<z; h++){
        if(g[h].empty()) continue;
        auto [m, fre, mul] = params[h];
        vector<float> window(m);
        for(int i=0; i<m; i++) window[i] = 0.25f - 0.25f * std::cos(2*PIF*i/m);
        for(int i=0; i+m<=n; i+=m/4){
            float sumi = 0.0f, sumo = 0.0f;
            for(auto [j, d] : g[h]){
                for(int k=0; k<m; k++) sumo += window[k] * norms[j][i+k] * d;
            }
            for(int k=0; k<m; k++) sumi += window[k] * norms[h][i+k];
            sumi /= m;
            sumo /= m * g[h].size();
            if(sumi == 0.0f) continue;
            float fact = sqrt(sumo / sumi);
            for(int j=0; j<m; j++) oband[h][i+j] += iband[h][i+j].real() * window[j] * fact;
        }
    }

    vector<float> out(n, 0.0f);
    for(auto &i : oband) for(int j=0; j<n; j++) out[j] += i[j];

    return out;

    */

    /*
    static unsigned f = 0;
    int pf = f;
    f = (f+1)%(windows.size()+1);

    if(pf == (int)windows.size()){
        vector<float> sum(n, 0.0f);
        for(int i=0; i<pf; i++){
            auto band = creal(calcband(i));
            for(int j=0; j<n; j++) sum[j] += band[j];
        }
        return sum;
    } else {
        return cabs(calcband(pf));
    }
    */

    return audio;
}

std::vector<float> energytranslate3(const std::vector<float> &audio){
    
    using std::vector;
    using std::tuple;
    using std::complex;

    int n = audio.size();
    const int N = 1<<11;
    float M = 500.0f;
    int m = std::ceil(M / (44100.0f / N));
    const int H = N/2+1;
    const float shift = 0.5f;

    vector<float> window(N), fwindow(2*m), out(n, 0.0f);
    for(int i=0; i<N; i++) window[i] = (1.0f - std::cos(2 * PIF * i / N)) / sqrt(6);
    for(int i=0; i<2*m; i++) fwindow[i] = (1.0f - std::cos(PIF * i / m));

    for(int i=0; i+N<=n; i+=N/4){
        vector<float> bit(N);
        for(int j=0; j<N; j++) bit[j] = audio[i+j] * window[j];
        auto freq = math::fft(bit);
        vector<float> inorm(H), onorm(H, 0.0f);
        for(int j=0; j<H; j++) inorm[j] = 2.0f * std::norm(freq[j]);
        inorm[0] *= 0.5f;
        for(int j=0; j<H; j++){
            int kb = std::round(j * shift - m);
            float fsum = 0.0f;
            for(int k=0, kk=kb; k<2*m; k++, kk++){
                if(kk >= 0 && kk < H) fsum += fwindow[k] * inorm[kk];
            }
            if(fsum != 0.0f) fsum = 1.0f / fsum;
            for(int k=0, kk=kb; k<2*m; k++, kk++){
                if(kk >= 0 && kk < H) onorm[kk] += fwindow[k] * inorm[kk] * fsum * inorm[j];
            }
        }
        for(int j=0; j<H; j++){
            float d = std::abs(freq[j]);
            if(d != 0.0f){
                freq[j] /= d;
                freq[j] *= std::sqrt(onorm[j] * 0.5f);
                if(j) freq[N-j] = std::conj(freq[j]);
                else freq[j] *= sqrt(2.0f);
            }
        }
        bit = math::inverse_fft(freq);
        for(int j=0; j<N; j++) out[i+j] += window[j] * bit[j];
    }

    /*
    auto filtered = out;
    for(float &i : out) i = 0.0f;
    
    for(int i=0; i+N<=n; i+=N/4){
        vector<float> bit(N);
        for(int j=0; j<N; j++) bit[j] = filtered[i+j] * window[j];
        auto freq = math::fft(bit);
        freq[0] = 0.0f;
        for(int j=1; j<2*m; j++) freq[j] = freq[N-j] = 0.0f;
        bit = math::inverse_fft(freq);
        for(int j=0; j<N; j++) out[i+j] += window[j] * bit[j];
    }
    */

    return out;
}

std::vector<float> hodgefilter(const std::vector<float> &audio){

    /*
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

    for(auto [x, y, xx, yy] : shifted){
        std::cerr << x << ' ' << y << ' ' << xx << ' ' << yy << '\n';
    }

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
    */

    return audio;
}

}

