#include "app/analyzer.h"
#include "app/app.h"
#include "math/fft.h"

#include <algorithm>

namespace app {

/*

Analyzer::Analyzer(ui::Window *master_, std::string title_) :
    Box(master_, title_),
    frame(master_),
    analyzerButtons(master_, {.height = 20}),
    analyzerWidgets(master_),
    graph(master_, {.look = "AnalyzerGraph"}),
    fileNameBox(master_, title_, {.look = "BoxTitle"}),
    switchRegular(master_, this, "time", {.look = "BoxLeftButton", .width = 50, .height = 20}),
    switchFrequency(master_, this, "frequency", {.look = "BoxLeftButton", .width = 50, .height = 20}),
    switchPeak(master_, this, "peak", {.look = "BoxLeftButton", .width = 50, .height = 20}),
    switchCorrelation(master_, this, "correlation",
            {.look = "BoxLeftButton", .width = 50, .height = 20})
{

    inner.put(1, 0, &frame);

    frame.setup_grid(3, 1);
    frame.fill_width({1});
    frame.fill_height({1, 0, 0});

    frame.put(0, 0, &graph);
    frame.put(1, 0, &analyzerButtons);
    frame.put(2, 0, &analyzerWidgets);

    analyzerButtons.setup_grid(1, 10);
    analyzerButtons.fill_height({1});
    analyzerButtons.fill_width(9, 1);

    analyzerButtons.put(0, 0, &switchRegular);
    analyzerButtons.put(0, 1, &switchFrequency);
    analyzerButtons.put(0, 2, &switchPeak);
    analyzerButtons.put(0, 3, &switchCorrelation);

    analyzerButtons.put(0, 9, &fileNameBox);
}

Analyzer::~Analyzer(){
    if(source != nullptr){
        delete source;
    }
}

int32_t Analyzer::switch_mode(Mode mode){
    
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

    return 0;
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

void Analyzer::SR::function(){
    Analyzer &a = *(Analyzer*)commander;
    a.switch_mode(Analyzer::regularMode);
}

void Analyzer::SF::function(){
    Analyzer &a = *(Analyzer*)commander;
    a.switch_mode(Analyzer::frequencyMode);
}

void Analyzer::SP::function(){
    Analyzer &a = *(Analyzer*)commander;
    a.switch_mode(Analyzer::peakMode);
}

void Analyzer::AC::function(){
    Analyzer &a = *(Analyzer*)commander;
    a.switch_mode(Analyzer::correlationMode);
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

int32_t Analyzer::link_audio(std::string fileName){
    
    return 0;

    App &app = *(App*)master;
    delete source;
    
    source = app.create_source(fileName);
    position = 0;
    length = defaultLength;

    fileNameBox.set_text(fileName);

    update_data();
    switch_mode(dataMode);

    return source != nullptr;
}

*/

}
