#include "tools/analyzer.h"
#include "app/app.h"
#include "math/fft.h"

#include <algorithm>

namespace app {

Analyzer::Analyzer(App *a) :
    Content(a),
    app(*a),
    slider(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    graph(a, {.look = "agraph", .border = {0, 0, 0, 0}}),
    buttons(a, {.height = 20}),
    fileNameBox(a, "", {.look = "basetext", .border = {0, 0, 0, 1}}),
    
    switchRegular(
            a,
            [&](){ switch_regular(); },
            "time",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}}),
    
    switchFrequency(
            a,
            [&](){ switch_frequency(); },
            "freq",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}}),
    
    switchPeak(
            a,
            [&](){ switch_peak(); },
            "peak",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}}),
    
    switchCorrelation(
            a,
            [&](){ switch_correlation(); },
            "corr",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}})
{

    setup_grid(2, 1);
    fill_width({1});
    fill_height({1, 0});

    put(0, 0, &graph);
    put(1, 0, &slider);

    slider.stack.create(2);
    slider.stack.put(&buttons, 0);
    slider.stack.put(&terminal, 1);
    slider.stack.update();
    slider.stack.fill_height({0, 1, 0});
    slider.stack.set_border(0, 0, 0, 0);

    buttons.setup_grid(1, 10);
    buttons.fill_height({1});
    buttons.fill_width(9, 1);

    buttons.put(0, 0, &switchRegular);
    buttons.put(0, 1, &switchFrequency);
    buttons.put(0, 2, &switchPeak);
    buttons.put(0, 3, &switchCorrelation);

    buttons.put(0, 9, &fileNameBox);

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("link", [&](ui::Command c){ link_audio(c); });

    terminal.document("link", "[name] link audio to this analyzer");
}

Analyzer::~Analyzer(){
    if(source != nullptr){
        delete source;
    }
}

std::string Analyzer::content_type(){ return "analyze"; }

void Analyzer::save(Saver &saver){

    saver.write_string(sourceName);
    saver.write_int(position);
    saver.write_int(length);
    saver.write_int(dataMode);

    graph.save(saver);
}

void Analyzer::load(Loader &loader){
    
    sourceName = loader.read_string();
    position = loader.read_int();
    length = loader.read_int();
    dataMode = (Mode)loader.read_int();

    switch_mode(dataMode);

    link_audio({terminal, {sourceName}});

    graph.load(loader);
}

void Analyzer::switch_mode(Mode mode){
    
    dataMode = mode;
    update_data();
    
    if(dataMode == regularMode){
        
        graph.fit_x();
        graph.fit_y();
        graph.set_unit_x("");
        graph.set_unit_y("");
        graph.refresh_all();
    
    } else if(dataMode == frequencyMode){
        
        graph.fit_x();
        graph.fit_y();
        graph.set_unit_x("Hz");
        graph.set_unit_y("");
        graph.refresh_all();

    } else if(dataMode == peakMode){

        graph.fit_x();
        graph.fit_y();
        graph.set_unit_x("");
        graph.set_unit_y("");
        graph.refresh_all();

    } else if(dataMode == correlationMode){

        graph.fit_x();
        graph.fit_y();
        graph.set_unit_x("");
        graph.set_unit_y("");
        graph.refresh_all();

    }
}

void Analyzer::update_data(){

    if(source == nullptr){

        graph.set_data(std::vector<float>(length, 0.0f));

    } else {

        if(dataMode == regularMode){
            
            graph.set_data(source->get(length, position));
            graph.set_offset_x(position);
            graph.set_scalar_x(1);
            graph.refresh_all();
        
        } else if(dataMode == frequencyMode){
            
            auto data = math::fft(source->get(length, position));
            data.resize(data.size()/2 + 1);

            graph.set_data(data);
            graph.set_offset_x(0);
            graph.set_scalar_x((double)source->frameRate / length);
            graph.refresh_all();
            
        } else if(dataMode == peakMode){

            graph.set_data(pitch.graph(source->get(length, position)));
            graph.set_offset_x(0);
            graph.set_scalar_x(1);
            graph.refresh_all();

        } else if(dataMode == correlationMode){
            
            graph.set_offset_x(0);
            graph.set_scalar_x(1);
            graph.refresh_all();

        }
    }

}

ui::Frame::Capture Analyzer::on_event(sf::Event event, int32_t priority){

    if(event.type == sf::Event::KeyPressed){
        
        if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D){
            
            int32_t speed = 1<<4;
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed = speed / (1<<4) * source->frameRate * source->channels;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 1<<4;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 1<<4;

            position = std::max(0, position + speed);

            update_data();

            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S){

            if(event.key.code == sf::Keyboard::S)
                length = std::max(1, length / 2);
            else 
                length = std::min(1<<20, length * 2);

            update_data();

            return Capture::capture;
        }
    
    }

    return Capture::pass;
}


void Analyzer::switch_regular(){
    switch_mode(regularMode);
}

void Analyzer::switch_frequency(){
    switch_mode(frequencyMode);
}

void Analyzer::switch_peak(){
    switch_mode(peakMode);
}

void Analyzer::switch_correlation(){
    switch_mode(correlationMode);
}

void Analyzer::link_audio(ui::Command c){
    
    sourceName = c.pop();

    if(source != nullptr) delete source;
    source = app.audio.get_source(sourceName);

    position = 0;

    update_data();
    switch_mode(dataMode);

}

struct initAnalyzer {
    initAnalyzer(){ Slot::add_content_type("analyze", [&](App *a){ return new Analyzer(a); } ); }
} iAnalyzer;

}
