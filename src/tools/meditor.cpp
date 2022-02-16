#include "tools/meditor.h"
#include "app/app.h"
#include "math/fft.h"

#include <math.h>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace app {

Meditor::Meditor(App *a) : 
    Content(a),
    app(*a),
    linkId(app.audio.generate_reciever_id()),
    outputName(linkId),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}})
{
    setup_grid(1, 1);
    fill_width(0, 1);
    fill_height(0, 1);

    put(0, 0, &terminal);

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("link", [&](ui::Command c){ link_audio(c); });
    terminal.put_function("name", [&](ui::Command c){ rename_output(c); });
    terminal.put_function("resize", [&](ui::Command c){ resize_matrix(c); });
    terminal.put_function("unit", [&](ui::Command c){ unit_matrix(c); });
    terminal.put_function("shuffle", [&](ui::Command c){ shuffle_matrix(c); });

    terminal.document("link", "[name] link audio to be edited."
            "Linked audio should be a single wavelength");
    terminal.document("name", "[name] set output name");
    terminal.document("resize", "resize edit matrix.");
    terminal.document("unit", "make edit matrix a unit matrix.");
    terminal.document("shuffle", "shuffle the phase of matrix cells.");
}

Meditor::~Meditor(){
    if(source != nullptr){
        if(app.audioAlive) app.audio.detach_source(sourceName, linkId);
        delete source;
    }
}

void Meditor::save(Saver &saver){

    saver.write_string(sourceName);
    saver.write_string(outputName);

    saver.write_unsigned(matrix.size());
    
    for(unsigned i=0; i<matrix.size(); i++){
        for(unsigned j=0; j<matrix.size(); j++){
            saver.write_float(matrix(i, j).real());
            saver.write_float(matrix(i, j).imag());
        }
    }
}

void Meditor::load(Loader &loader){
    
    sourceName = loader.read_string();
    outputName = loader.read_string();
    
    link_audio({terminal, {sourceName}});
    
    matrix.resize(loader.read_unsigned());
    
    for(unsigned i=0; i<matrix.size(); i++){
        for(unsigned j=0; j<matrix.size(); j++){
            std::complex<float> c{loader.read_float(), loader.read_float()};
            matrix.set(i, j, c);
        }
    }
}

std::string Meditor::content_type(){ return type; }

const std::string Meditor::type = "medit";

int Meditor::init_class = [](){
    Slot::add_content_type(type, [](App *a){ return new Meditor(a); });
    return 0;
}();

void Meditor::on_tick(){
    
    if(!sourceName.empty() && app.audio.pop_update(sourceName, linkId)){

        wave::Source *src = app.audio.get_source(sourceName, linkId);
    
        if(src){
            if(source != nullptr) delete source;
            source = src;
            update_output();
        }
    }
}

void Meditor::update_output(){
    
    if(source == nullptr) return;

    auto data = source->get(source->size(), 0);
    
    auto freq = math::bluestein(data);

    std::vector<std::complex<float> > edits(matrix.size(), 0.0f);
    for(unsigned i=1; i <= freq.size() / 2 && i <= matrix.size(); i++) edits[i-1] = freq[i];

    edits = matrix*edits;

    freq = std::vector<std::complex<float> >(freq.size(), 0.0f);
    for(unsigned i=1; i <= freq.size() / 2 && i <= matrix.size(); i++){
        freq[freq.size() - i] = std::conj(edits[i-1]);
        freq[i] = edits[i-1];
    }

    data = math::inverse_bluestein(freq);
    
    wave::Audio *audio = new wave::Audio();
    audio->name = outputName;
    audio->channels = source->channels;
    audio->frameRate = source->frameRate;
    audio->data = data;

    app.audio.add_cache(audio);
}

void Meditor::resize_matrix(ui::Command c){
    
    std::stringstream s(c.pop());
    unsigned size;
    s >> size;

    matrix.resize(size);

    update_output();
}

void Meditor::shuffle_matrix(ui::Command c){
    matrix.phase_shuffle();
    update_output();
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
        c.source.push_error("name cannot be empty");
    }

}

void Meditor::link_audio(ui::Command c){

    sourceName = c.pop();

    wave::Source *src = app.audio.get_source(sourceName, linkId);

    if(src){
        if(source != nullptr) delete source;
        source = src;
        update_output();
    }
    else {
        c.source.push_error("audio source not found");
    }
}

}
