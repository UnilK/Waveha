#include "designa/color.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>

namespace designa {

using std::valarray;
using std::vector;
using std::complex;

vector<float> pick_window_from_buffer(
        const vector<float> &window,
        const valarray<float> &buffer,
        int offset, int size){
    
    vector<float> bit(size, 0.0f);
    int mid = size / 2;
    int half = window.size() / 2;
    
    for(int i=0; i<(int)window.size(); i++){
        assert(mid - half + i >= 0 && mid - half + i < size);
        assert(offset + i >= 0 && offset + i < (int)buffer.size());
        bit[mid - half + i] = buffer[offset + i] * window[i];
    }
    
    return bit;
}

void apply_window_to_buffer(
        const std::vector<float> &bit,
        const std::vector<float> &window,
        std::valarray<float> &buffer,
        int offset){

    int mid = bit.size() / 2;
    int half = window.size() / 2;
    
    for(int i=0; i<(int)window.size(); i++){
        assert(mid - half + i >= 0 && mid - half + i < (int)bit.size());
        assert(offset + i >= 0 && offset + i < (int)buffer.size());
        buffer[offset + i] += bit[mid - half + i]  * window[i];
    }
}

vector<complex<float> > frequency_phases(
        const vector<complex<float> > &frequency){

    vector<complex<float> > phase(frequency.size());
    for(unsigned i=0; i<phase.size(); i++) phase[i] = unit_complex(frequency[i]);
    return phase;
}

vector<float> frequency_energies(
        const vector<complex<float> > &frequency){
    
    vector<float> energy(frequency.size());
    for(unsigned i=0; i<energy.size(); i++) energy[i] = std::norm(frequency[i]);
    return energy;
}

vector<complex<float> > join_energy_to_phase(
        const vector<float> &energy,
        vector<complex<float> > phase){
    for(unsigned i=0; i<phase.size(); i++) phase[i] *= std::sqrt(energy[i]);
    return phase;
}

void add_phase_noise(
        vector<complex<float> > &phase,
        const vector<float> &noise_magnitude){
    
    for(unsigned i=0; i<phase.size(); i++){
        phase[i] *= std::polar<float>(1.0f, M_PI * rnd(noise_magnitude[i]));
    }
}

vector<float> color_injection(
        const vector<float> &source_color,
        const vector<float> &source_pattern,
        const vector<float> &color_shift,
        float color_step,
        float pattern_step){
    
    int size = source_color.size();
    int color_size = std::ceil(size / color_step) + 1;
    int pattern_size = std::ceil(size / pattern_step) + 1;

    auto extract_window = [](const vector<float> &source, float point, float width) -> float {
        const int l = std::max<int>(0, std::ceil(point - width));
        const int r = std::min<int>(source.size()-1, std::floor(point + width));
        const float len = M_PI / width;
        float sum = 0.0f;
        for(int i=l; i<=r; i++){
            sum += source[i] * (0.5f + 0.5f * std::cos(len * (i - point)));
        }
        return sum;
    };

    vector<float> color_bin(color_size, 0.0f), pattern_bin(pattern_size, 0.0f);
    
    for(int i=0; i<color_size; i++){
        color_bin[i] = extract_window(source_color, i * color_step, color_step);
    }
    
    for(int i=0; i<pattern_size; i++){
        pattern_bin[i] = extract_window(source_color, i * pattern_step, pattern_step);
    }

    vector<float> output(size, 0.0f), output_bin(pattern_size, 0.0f);

    auto exact_color_shift = [&](float x){
        const int l = std::max<int>(0, std::ceil(x));
        const int r = std::min<int>(size-1, std::floor(x));
        const float d = std::max(0.0f, std::min(x-l, 1.0f));
        return color_shift[l] * (1.0f - d) + color_shift[r] * d;
    };

    {
        const float width = std::max<float>(color_step, pattern_step);
        const float len = M_PI / width;

        for(int i=0; i<color_size; i++){
            const float point = i * color_step * exact_color_shift(i * color_step);
            const int l = std::max<int>(0, std::ceil((point-width) / pattern_step));
            const int r = std::min<int>(pattern_size-1, std::floor((point+width) / pattern_step));
            float sum = 0.0f;
            for(int j=l; j<=r; j++){
                sum += 0.5f + 0.5f * std::cos(len * (j * pattern_step - point));
            }
            sum = 1.0f / (sum + 1e-18f);
            for(int j=l; j<=r; j++){
                output_bin[j] += (0.5f + 0.5f * std::cos(len * (j * pattern_step - point)))
                                * sum * color_bin[i];
            }
        }
    }

    auto apply_window = [](vector<float> &destination, const vector<float> &pattern,
            float point, float width, float ratio) -> void {

        const int l = std::max<int>(0, std::ceil(point - width));
        const int r = std::min<int>(destination.size()-1, std::floor(point + width));
        const float len = M_PI / width;
        for(int i=l; i<=r; i++){
            destination[i] += ratio * pattern[i] * (0.5f + 0.5f * std::cos(len * (i - point)));
        }
    };

    for(int i=0; i<pattern_size; i++){
        apply_window(output, source_pattern, i * pattern_step, pattern_step,
                output_bin[i] / (pattern_bin[i] + 1e-18f));
    }

    return output;
}


}

