#include "tools/meditor.h"
#include "app/app.h"
#include "math/fft.h"
#include "math/ft.h"
#include "math/constants.h"
#include "app/audio.h"
#include "app/creations.h"
#include "change/pitch.h"

#include <math.h>
#include <algorithm>
#include <iostream>

namespace app {

Meditor::Meditor(App *a) : 
    Content(a),
    app(*a),
    link(*a),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}})
{
    matrix.resize(64);

    setup_grid(1, 1);
    fill_width(0, 1);
    fill_height(0, 1);

    put(0, 0, &terminal);
    
    terminal.put_directory("au", &app.audio.dir);

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("link", [&](ui::Command c){ link_audio(c); });
    terminal.put_function("name", [&](ui::Command c){ rename_output(c); });
    terminal.put_function("resize", [&](ui::Command c){ resize_matrix(c); });
    terminal.put_function("unit", [&](ui::Command c){ unit_matrix(c); });
    terminal.put_function("phasesuf", [&](ui::Command c){ shuffle_phase(c); });
    terminal.put_function("magsuf", [&](ui::Command c){ shuffle_magnitude(c); });
    terminal.put_function("phaseset", [&](ui::Command c){ set_phase(c); });
    terminal.put_function("magset", [&](ui::Command c){ set_magnitude(c); });
    terminal.put_function("stack", [&](ui::Command c){ set_stack(c); });
    terminal.put_function("mode", [&](ui::Command c){ set_mode(c); });
    terminal.put_function("auto", [&](ui::Command c){ set_auto(c); });
    terminal.put_function("slant", [&](ui::Command c){ set_slant(c); });
    terminal.put_function("pitch", [&](ui::Command c){ set_pitch(c); });
    terminal.put_function("reme", [&](ui::Command c){ remember(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });

    terminal.document("link", "[name] link audio to be edited."
            " Linked audio should be a single period of a wave");
    terminal.document("name", "[name] set output name");
    terminal.document("resize", "resize edit matrix.");
    terminal.document("unit", "make edit matrix a unit matrix.");
    terminal.document("phasesuf", "[factor] shuffle the phase of matrix cells.");
    terminal.document("magsuf", "[factor] shuffle the magnitude of matrix cells.");
    terminal.document("phaseset", "[low] [high] [angle] rotate frequencies in range [low, high[ by angle.");
    terminal.document("magset", "[low] [high] [factor] multply frequencies in ragne [low, high[ by angle.");
    terminal.document("stack", "[stack] use this stack to transform the waves.");
    terminal.document("mode", "[mode] use this transformation mode.");
    terminal.document("auto", "switch auto auto");
    terminal.document("slant", "[in] [out] multiply wave by a linear function");
    terminal.document("pitch", "[factor] multiply wavelength by factor");
    terminal.document("reme", "remember magnitudes from current input");
    terminal.document("info", "list configuration.");
}

Meditor::~Meditor(){}

void Meditor::save(ui::Saver &saver){

    saver.write_string(link.source());
    saver.write_string(stack);
    saver.write_int(mode);
    saver.write_string(outputName);

    saver.write_unsigned(matrix.size());
    
    for(unsigned i=0; i<matrix.size(); i++){
        for(unsigned j=0; j<matrix.size(); j++){
            saver.write_complex(matrix(i, j));
        }
    }
}

void Meditor::load(ui::Loader &loader){
    
    std::string sourceName = loader.read_string();
    stack = loader.read_string();
    mode = loader.read_int();
    outputName = loader.read_string();
    
    link_audio({terminal, {sourceName}});
    
    matrix.resize(loader.read_unsigned());

    for(unsigned i=0; i<matrix.size(); i++){
        for(unsigned j=0; j<matrix.size(); j++){
            matrix.set(i, j, loader.read_complex());
        }
    }

    update_output();
}

namespace Factory { extern std::string meditor; }
std::string Meditor::content_type(){ return Factory::meditor; }

void Meditor::on_tick(){
    if(link.pop_update() || autoRefresh) update_output();
}

void Meditor::update_output(){
   
    auto data = link.get(link.size(), 0);
    
    if(mode == 0){
        
        for(unsigned i=0; i<data.size(); i++){
            float d = (float)i/data.size();
            data[i] /= (1-d)+d*slantIn;
        }
        
        auto freq = math::ft(data, matrix.size());

        freq = matrix*freq;

        data = math::ift(freq, (unsigned)std::round(data.size() / pitch));
        
        for(unsigned i=0; i<data.size(); i++){
            float d = (float)i/data.size();
            data[i] *= (1-d)+d*slantOut;
        }
    }
    else if(mode == 1){
        data = change::ml_graph(app.creations.get_stack(stack), data, pitch);
    }
    else if(mode == 2){
       
        const int N = 64;
        const int D = 8;
        const int M = 16;

        auto freq = math::ft(data, N);
        
        auto nfreq = freq;
        for(int i=M; i<N; i++) nfreq[i] = 0.0f;
        for(unsigned i=D; i<M; i++) nfreq[i] *= 0.666666f;

        for(int i=D; i+M<=N; i+=D){
            
            auto tmp = freq;
            for(int j=0; j<N; j++) if(j<i || j>=i+M) tmp[j] = 0.0f;
            for(int j=0; j<M; j++) tmp[j] = tmp[j+i];

            auto ttmp = math::ift(tmp, data.size());
            for(float &i : ttmp) i = std::abs(i);
            tmp = math::ft(ttmp, N);

            for(int j=0; j<M; j++) nfreq[i+j] = tmp[j];

        }

        data = math::ift(nfreq, data.size());
    }
    else if(mode == 3){
        
        auto freq = math::ft(data, matrix.size());

        auto mag = rmags;
        mag.resize(matrix.size(), 0.0f);

        for(unsigned i=0; i<freq.size(); i++){
            float d = std::abs(freq[i]);
            if(d != 0.0f) freq[i] *= mag[i] / d;
        }

        data = math::ift(freq, (unsigned)std::round(data.size() / pitch));

    } else if(mode == 4){
        
        data = change::pitch_changer(data, pitch);

    } else if(mode == 5){

        const int N = 64;

        auto freq = math::ft(data, N);
        freq = change::translate(freq, pitch);

        data = math::ift(freq, (unsigned)std::round(data.size() / pitch));

    } else if(mode == 6){
        
        const int N = 1024;
        
        data.resize(N);
        for(int i=0; i<N; i++) data[i] = (-100.0f * (N - i) + 0.0f * i) / N;
        for(float &i : data) i = std::pow(2.0f, i);

    } else if(mode == 7){

    }
    
    wave::Audio *audio = new wave::Audio();
    audio->name = outputName;
    audio->channels = link.channels;
    audio->frameRate = link.frameRate;
    audio->data = data;

    app.audio.add_cache(audio);
}

void Meditor::resize_matrix(ui::Command c){
    try {
        matrix.resize(std::stoi(c.pop()));
        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::shuffle_phase(ui::Command c){
    try {

        float factor = std::stof(c.pop());

        for(unsigned i=0; i<matrix.size(); i++){
            for(unsigned j=0; j<matrix.size(); j++){
                matrix.multiply(i, j,
                        std::polar(1.0f, (float)rand() / RAND_MAX * 2.0f * PIF * factor));
            }
        }

        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::shuffle_magnitude(ui::Command c){
    try {

        float high = std::stof(c.pop());
        float low = 1.0f / high;

        for(unsigned i=0; i<matrix.size(); i++){
            for(unsigned j=0; j<matrix.size(); j++){
                matrix.multiply(i, j, low + (high-low) * rand() / RAND_MAX);
            }
        }

        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::set_phase(ui::Command c){

    try {

        unsigned low = std::stoul(c.pop());
        unsigned high = std::stoul(c.pop());
        float phase = std::stof(c.pop());

        high = std::min(high, (unsigned)matrix.size());
        std::complex<float> offset = std::polar(1.0f, phase*2*PIF);

        for(unsigned i=low; i<high; i++){
            matrix.multiply(i, i, offset);
        }

        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::set_magnitude(ui::Command c){

    try {

        unsigned low = std::stoul(c.pop());
        unsigned high = std::stoul(c.pop());
        float offset = std::stof(c.pop());

        high = std::min(high, (unsigned)matrix.size());

        for(unsigned i=low; i<high; i++){
            matrix.multiply(i, i, offset);
        }

        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::unit_matrix(ui::Command c){
    matrix.set_unit();
    update_output();
}

void Meditor::rename_output(ui::Command c){
    
    auto newName = c.pop();
    
    if(!newName.empty()){
        outputName = newName;
        update_output();
    }
    else {
        c.source.push_error("give a name");
    }

}

void Meditor::link_audio(ui::Command c){

    std::string newName = c.pop();

    if(!newName.empty()){
        bool found = link.open(newName);
        update_output();
        if(!found) c.source.push_error("source not found: " + newName);
    }
    else {
        c.source.push_error("give a name");
    }
}

void Meditor::set_stack(ui::Command c){
    stack = c.pop();
    update_output();
}

void Meditor::set_mode(ui::Command c){
    try {
        mode = std::stoi(c.pop());
        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::info(ui::Command c){
    std::string message =
        "in: " + link.source() + "\n"
        + "size: " + std::to_string(matrix.size()) + "\n"
        + "out: " + outputName + "\n"
        + "stack: " + stack + "\n"
        + "mode: " + std::to_string(mode);
    c.source.push_output(message);
}

void Meditor::set_slant(ui::Command c){
    try {
        slantIn = std::stof(c.pop());
        slantOut = std::stof(c.pop());
        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::set_pitch(ui::Command c){
    try {
        pitch = std::stof(c.pop());
        update_output();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Meditor::remember(ui::Command c){
    
    auto data = link.get(link.size(), 0);
    
    auto freq = math::ft(data, matrix.size());

    rmags.resize(matrix.size());
    for(unsigned i=0; i<matrix.size(); i++) rmags[i] = std::abs(freq[i]);

}

void Meditor::set_auto(ui::Command c){
    autoRefresh ^= 1;
}

}
