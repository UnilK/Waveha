#include "designa/color.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>

namespace designa {

using std::valarray;
using std::vector;
using std::complex;

vector<float> pick_bit(
        const valarray<float> &buffer,
        int offset, int size)
{    
    assert(offset >= 0 && offset + size <= (int)buffer.size());

    vector<float> bit(size);
    for(int i=0; i<size; i++) bit[i] = buffer[offset + i];

    return bit;
}

void add_bit(
        const vector<float> &bit,
        valarray<float> &buffer,
        int offset)
{    
    int size = bit.size();
    assert(offset >= 0 && offset + size <= (int)buffer.size());

    for(int i=0; i<size; i++) buffer[offset + i] += bit[i];
}

void apply_window(
        vector<float> &bit,
        const vector<float> &window)
{
    assert(bit.size() >= window.size());

    for(unsigned i=0; i<window.size(); i++) bit[i] *= window[i];
    for(unsigned i=window.size(); i<bit.size(); i++) bit[i] = 0.0f;
}

vector<complex<float> > frequency_phases(
        const vector<complex<float> > &frequency)
{
    vector<complex<float> > phase(frequency.size());
    for(unsigned i=0; i<phase.size(); i++) phase[i] = unit_complex(frequency[i]);
    return phase;
}

vector<float> frequency_energies(
        const vector<complex<float> > &frequency)
{    
    vector<float> energy(frequency.size());
    for(unsigned i=0; i<energy.size(); i++) energy[i] = std::norm(frequency[i]);
    return energy;
}

vector<complex<float> > join_energy_to_phase(
        const vector<float> &energy,
        vector<complex<float> > phase)
{    
    assert(energy.size() == phase.size());

    for(unsigned i=0; i<phase.size(); i++) phase[i] *= std::sqrt(energy[i]);
    return phase;
}

std::vector<std::complex<float> > merge_frequencies(
        std::vector<std::complex<float> > freq1,
        const std::vector<std::complex<float> > &freq2,
        const std::vector<float> &factor)
{   
    assert(freq1.size() == freq2.size());
    assert(freq1.size() == factor.size());

    for(unsigned i=0; i<freq1.size(); i++){
        freq1[i] = freq1[i] * (1.0f - factor[i]) + freq2[i] * factor[i];
    }

    return freq1;
}

void add_phase_noise(
        vector<complex<float> > &phase,
        const vector<float> &noise_magnitude)
{   
    assert(phase.size() == noise_magnitude.size());

    for(unsigned i=0; i<phase.size(); i++){
        phase[i] *= std::polar<float>(1.0f, M_PI * rnd(noise_magnitude[i]));
    }
}

vector<float> split_to_bins(
        const vector<float> &energy,
        float bin_size)
{   
    const int n = energy.size();
    const int m = std::ceil(n / bin_size) + 1;
    const float speed = M_PI / bin_size;
    
    auto extract_window = [&](float point) -> float {
        const int l = std::max<int>(0, std::ceil(point - bin_size));
        const int r = std::min<int>(n-1, std::floor(point + bin_size));
        float sum = 0.0f;
        for(int i=l; i<=r; i++){
            sum += energy[i] * (0.5f + 0.5f * std::cos(speed * (i - point)));
        }
        return sum;
    };

    vector<float> bin(m);

    for(int i=0; i<m; i++) bin[i] = extract_window(i * bin_size);

    return bin;
}

vector<float> shift_bins(
        const std::vector<float> &bin,
        const std::vector<float> &shift)
{
    assert(bin.size() == shift.size());

    int n = bin.size();
    std::vector<float> result(n, 0.0f);

    result[0] = bin[0];

    for(int i=0; i<n; i++){
        
        const float j = i * shift[i];
        const float width = std::max(1.0f, shift[i]);
        const float w = M_PI / width;

        const int l = std::max<int>(1, std::ceil(j - width));
        const int r = std::min<int>(n-1, std::floor(j + width));

        float sum = 0.0f;
        for(int k=l; k<=r; k++) sum += 0.5f + 0.5f * std::cos(w * (j - k));
    
        sum = 1.0f / (sum + 1e-18f);
        
        for(int k=l; k<=r; k++)
            result[k] += (0.5f + 0.5f * std::cos(w * (j - k))) * sum * bin[i];
    }

    return result;
}

}

