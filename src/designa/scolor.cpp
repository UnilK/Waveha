#include "designa/scolor.h"
#include "designa/color.h"
#include "designa/math.h"

#include <cassert>
#include <cmath>

namespace designa {

Scolor::Scolor(int framerate, float min_frequency, float decay_halftime, int window_steps){
    
    step = window_steps;
    assert("window steps must be a multiple of 4" && step % 4 == 0 && step > 0);

    decay = std::pow(2.0f, 1.0f / (framerate * decay_halftime));

    wsize = std::ceil(framerate / min_frequency);
    wsize = (wsize + step - 1) / step * step;
    size = 4 * wsize;

    psize = 1;
    while(psize < wsize) psize *= 2;
    
    wsize += wsize / step;

    pointer = 0;
    state = 0;

    ibuff.resize(size, 0.0f);
    obuff.resize(size, 0.0f);
    pbuff.resize(size, wsize / 2.0f);

    shift.resize(psize / 2 + 1, 1.0f);
}

float Scolor::process(float sample, float period){
    
    if(pointer + wsize + wsize / step > size){
        ibuff = ibuff.shift(pointer);
        obuff = obuff.shift(pointer);
        pbuff = pbuff.shift(pointer);
        pointer = 0;
    }

    ibuff[pointer + wsize - 1] = sample;
    
    period = std::max<float>(pbuff[pointer + wsize - 2] / decay,
            std::min(period, pbuff[pointer + wsize - 2] * decay));

    pbuff[pointer + wsize - 1] = period;

    if(state == 0){
        
        int width = std::ceil(pbuff[pointer]);
        width = (step + width) / step  * step;

        int pwidth = 1;
        while(pwidth < width) pwidth *= 2;

        auto window = cos_window(width / 2, width / 2);
        
        std::vector<float> bit1(width), bit2(width);
        for(int i=0; i<width; i++){
            bit1[i] = ibuff[pointer + i] * window[i];
            bit2[i] = ibuff[pointer + width / step + i] * window[i];
        }

        bit1.resize(pwidth, 0.0f);
        bit2.resize(pwidth, 0.0f);

        auto [freq1, freq2] = fft(bit1, bit2);
        
        auto phase1 = frequency_phases(freq1);
        auto phase2 = frequency_phases(freq2);
        auto energy1 = frequency_energies(freq1);
        auto energy2 = frequency_energies(freq2);

        int fsize = freq1.size();
        std::vector<float> shift_subset(fsize);

        {
            int step = psize / pwidth;
            for(int i=0; i<fsize; i++) shift_subset[i] = shift[i * step];
        }

        energy1 = shift_bins(energy1, shift_subset);
        energy2 = shift_bins(energy2, shift_subset);

        freq1 = join_energy_to_phase(energy1, phase1);
        freq2 = join_energy_to_phase(energy2, phase2);

        auto [out1, out2] = inverse_fft(freq1, freq2);

        for(int i=0; i<width; i++){
            obuff[pointer + i] += out1[i] * window[i];
            obuff[pointer + width / step + i] += out2[i] * window[i];
        }
        
        state = 2 * width / step;
    }

    state--;

    return obuff[pointer++] * 8 / (step * 3);
}

int Scolor::get_shift_size(){ return psize / 2 + 1; }

void Scolor::set_shift(const std::vector<float> &new_shift){
    assert(new_shift.size() == shift.size());
    shift = new_shift;
}

int Scolor::get_delay(){ return wsize; }

}

