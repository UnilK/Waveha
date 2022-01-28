#include "tools/analyzer.h"
#include "app/app.h"
#include "app/slot.h"
#include "math/fft.h"

#include <algorithm>

namespace app {

Analyzer::Analyzer(App *a) :
    ui::Frame(a),
    app(*a),
    slider(a, ui::Side::up, ui::Side::down, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal"}),
    graph(a, {.look = "agraph"}),
    buttons(a, {.height = 20}),
    fileNameBox(a, "", {.look = "basetext"}),
    
    switchRegular(
            a,
            [&](){ switch_regular(); },
            "time",
            {.look = "basebutton", .width = 50}),
    
    switchFrequency(
            a,
            [&](){ switch_frequency(); },
            "freq",
            {.look = "basebutton", .width = 50}),
    
    switchPeak(
            a,
            [&](){ switch_peak(); },
            "peak",
            {.look = "basebutton", .width = 50}),
    
    switchCorrelation(
            a,
            [&](){ switch_correlation(); },
            "corr",
            {.look = "basebutton", .width = 50})
{

    setup_grid(2, 1);
    fill_width({1});
    fill_height({1, 0});

    put(0, 0, &graph);
    put(1, 0, &slider);

    slider.stack.push_back(&buttons);
    slider.stack.push_back(&terminal);

    buttons.setup_grid(1, 10);
    buttons.fill_height({1});
    buttons.fill_width(9, 1);

    buttons.put(0, 0, &switchRegular);
    buttons.put(0, 1, &switchFrequency);
    buttons.put(0, 2, &switchPeak);
    buttons.put(0, 3, &switchCorrelation);

    buttons.put(0, 9, &fileNameBox);

    terminal.put_function("link", [&](ui::Command c){ link_audio(c); });
}

Analyzer::~Analyzer(){
    if(source != nullptr){
        delete source;
    }
}

void Analyzer::save(Saver&){

}

void Analyzer::load(Loader&){

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
    
    std::string src = c.pop();

    if(source != nullptr) delete source;
    source = app.audio.get_source(src);

    update_data();
    switch_mode(dataMode);

}

struct init {
    init(){ Slot::add_content_type("analyze", [&](App *a){ return new Analyzer(a); } ); }
} i;

}
