#include "app/analyzer.h"
#include "app/app.h"
#include "math/fft.h"

#include <algorithm>

namespace app {

Analyzer::Analyzer(ui::Window *master_, std::string title_) :
    Box(master_, title_),
    Commandable(title_),
    frame(master_),
    analyzerButtons(master_, kwargs{{"height", "20"}}),
    analyzerWidgets(master_),
    graph(master_, kwargs{{"look", "AnalyzerGraph"}}),
    fileNameBox(master_, kwargs
            {{"text", "no linked file"},
            {"look", "BoxTitle"}}),
    switchRegular(master_, this, kwargs
            {{"text", "time"},
            {"width", "50"},
            {"look", "BoxLeftButton"}}),
    switchFrequency(master_, this, kwargs
            {{"text", "frequency"},
            {"height", "20"},
            {"width", "100"},
            {"look", "BoxLeftButton"}}),
    switchPeak(master_, this, kwargs
            {{"text", "peak"},
            {"width", "50"},
            {"look", "BoxLeftButton"}}),
    switchCorrelation(master_, this, kwargs
            {{"text", "correlation"},
            {"width", "120"},
            {"look", "BoxLeftButton"}})
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

    commandHelp = "audio analyzer";
    commandDocs = {
        {"link _name", "set analyzed audio"}};
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
    
    } else if(dataMode == frequencyMode){
        
        graph.fit_x();
        graph.fit_y();

    } else if(dataMode == peakMode){

        graph.fit_x();
        graph.fit_y();

    } else if(dataMode == correlationMode){

        graph.fit_x();
        graph.fit_y();

    }

    return 0;
}

void Analyzer::update_data(){

    if(source == nullptr){

        graph.set_data(std::vector<float>(length, 0.0f));

    } else {

        if(dataMode == regularMode){
            
            graph.set_data(source->get(length, position));
        
        } else if(dataMode == frequencyMode){
            
            auto data = math::fft(source->get(length, position));
            data.resize(data.size()/2 + 1);

            graph.set_data(data);
            
        } else if(dataMode == peakMode){

            graph.set_data(pitch.graph(source->get(length, position)));

        } else if(dataMode == correlationMode){
            
            

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

int32_t Analyzer::execute_command(ui::Command cmd){

    if(execute_standard(cmd)) return 0;

    App &app = *(App*)core;
    
    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;

    if(prefix == "link"){
        
        std::string handle;
        cin >> handle;

        app.log_command(cmd);

        return link_audio(handle);
    }

    return 0;
}

int32_t Analyzer::on_event(sf::Event event, int32_t priority){
   
    if(event.type == sf::Event::KeyPressed){
        
        if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D){
            
            int32_t speed = 1<<4;
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed = speed / 1<<4 * source->frameRate * source->channels;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 1<<4;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 1<<4;

            position = std::max(0, position + speed);

            update_data();

            return 1;

        } else if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S){

            if(event.key.code == sf::Keyboard::S)
                length = std::max(1, length / 2);
            else 
                length = std::min(1<<20, length * 2);

            update_data();

            return 1;
        }
    
    }

    return -1;
}

int32_t Analyzer::link_audio(std::string fileName){
    
    App &app = *(App*)core;
    delete source;
    
    source = app.create_source(fileName);
    position = 0;
    length = defaultLength;

    update_data();
    switch_mode(dataMode);

    return source != nullptr;
}

}
