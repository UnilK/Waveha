#include "tools/meditor.h"
#include "app/app.h"
#include "math/fft.h"
#include "math/ft.h"
#include "math/constants.h"
#include "app/audio.h"
#include "app/creations.h"

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
    terminal.put_function("stack", [&](ui::Command c){ set_stack(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });
    
    terminal.put_function("slant", [&](ui::Command c){ set_slant(c); });
    terminal.put_function("pitch", [&](ui::Command c){ set_pitch(c); });

    terminal.document("link", "[name] link audio to be edited."
            "Linked audio should be a single wavelength");
    terminal.document("name", "[name] set output name");
    terminal.document("resize", "resize edit matrix.");
    terminal.document("unit", "make edit matrix a unit matrix.");
    terminal.document("phasesuf", "shuffle the phase of matrix cells.");
    terminal.document("magsuf", "[low] [high] shuffle the magnitude of matrix cells.");
    terminal.document("stack", "[stack] use this stack to transform the waves.");
    terminal.document("info", "list configuration.");
}

Meditor::~Meditor(){}

void Meditor::save(ui::Saver &saver){

    saver.write_string(link.source());
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
    if(link.pop_update()) update_output();
}

void Meditor::update_output(){
    
    auto data = link.get(link.size(), 0);
    
    for(unsigned i=0; i<data.size(); i++){
        float d = (float)i/data.size();
        data[i] /= (1-d)+d*slantIn;
    }
    
    auto freq = math::ft(data, matrix.size());

    freq = matrix*freq;

    data = math::ift(freq, (unsigned)std::round(pitch*data.size()));
    
    for(unsigned i=0; i<data.size(); i++){
        float d = (float)i/data.size();
        data[i] *= (1-d)+d*slantOut;
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
    for(unsigned i=0; i<matrix.size(); i++){
        for(unsigned j=0; j<matrix.size(); j++){
            matrix.multiply(i, j, std::polar(1.0f, (float)rand()/RAND_MAX*2*PIF));
        }
    }
    update_output();
}

void Meditor::shuffle_magnitude(ui::Command c){
    try {
        float low = std::stof(c.pop());
        float high = std::stof(c.pop());

        for(unsigned i=0; i<matrix.size(); i++){
            for(unsigned j=0; j<matrix.size(); j++){
                if(i < 10) matrix.multiply(i, j, low + (high-low) * rand() / RAND_MAX);
            }
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

void Meditor::info(ui::Command c){
    std::string message =
        "in: " + link.source() + "\n"
        + "size: " + std::to_string(matrix.size()) + "\n"
        + "out: " + outputName;
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

}
