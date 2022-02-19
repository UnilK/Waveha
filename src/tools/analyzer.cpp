#include "tools/analyzer.h"
#include "app/app.h"
#include "app/audio.h"
#include "math/fft.h"

#include <algorithm>
#include <math.h>
#include <limits>
#include <iostream>

namespace app {

AnalyzerGraph::AnalyzerGraph(Analyzer &a, ui::Kwargs kwargs) :
    Graph((App*)a.get_master(), kwargs),
    beginLine(sf::LineStrip, 2),
    endLine(sf::LineStrip, 2),
    analyzer(a)
{
    set_look(look);
}

void AnalyzerGraph::set_look(std::string look_){
    Graph::set_look(look_);
    for(int i=0; i<2; i++) beginLine[i].color = color("axisColor");
    for(int i=0; i<2; i++) endLine[i].color = color("axisColor");
}

ui::Frame::Capture AnalyzerGraph::on_event(sf::Event event, int priority){

    if(Graph::on_event(event, priority) == Capture::capture){
        views[currentMode] = View{origoX, origoY, scaleX, scaleY};
        return Capture::capture;
    }

    if(event.type == sf::Event::KeyPressed){
        
        if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D){
            
            int speed = 16;
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed = speed / 16 * analyzer.source->frameRate * analyzer.source->channels;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;

            if(analyzer.loop != nullptr){
                analyzer.position = (analyzer.position + speed) % (int)analyzer.loop->size();
                if(analyzer.position < 0) analyzer.position += analyzer.loop->size();
                analyzer.update_data();
            }

            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S){

            if(event.key.code == sf::Keyboard::S)
                analyzer.length = std::max(1, analyzer.length / 2);
            else 
                analyzer.length = std::min(1<<18, analyzer.length * 2);

            analyzer.update_data();

            return Capture::capture;
        
        } else if(event.key.code == sf::Keyboard::I || event.key.code == sf::Keyboard::K){
           
            if(currentMode != regularMode || !analyzer.clipping || !analyzer.loop)
                return Capture::pass;

            int speed = 16;
            if(event.key.code == sf::Keyboard::K) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;

            clipEnd = (clipEnd + speed) % analyzer.loop->size();
            if(clipEnd < clipBegin) clipEnd += analyzer.loop->size();

            if(clipBegin != analyzer.clipBegin || clipEnd != analyzer.clipEnd){
                analyzer.clipBegin = clipBegin;
                analyzer.clipEnd = clipEnd;
                analyzer.save_clip();
                set_refresh();
            }
            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::J || event.key.code == sf::Keyboard::L){
            
            if(currentMode != regularMode || !analyzer.clipping || !analyzer.loop)
                return Capture::pass;
            
            int speed = 16;
            if(event.key.code == sf::Keyboard::J) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;

            int length = clipEnd - clipBegin;

            clipBegin = (clipBegin + speed) % (int)analyzer.loop->size();
            if(clipBegin < 0) clipBegin += analyzer.loop->size();
            
            clipEnd = clipBegin + length;
            
            if(clipBegin != analyzer.clipBegin || clipEnd != analyzer.clipEnd){
                analyzer.clipBegin = clipBegin;
                analyzer.clipEnd = clipEnd;
                analyzer.save_clip();
                set_refresh();
            }
            return Capture::capture;
        
        } else if(event.key.code == sf::Keyboard::U){
            
            if(currentMode != regularMode || !analyzer.clipping || !analyzer.loop)
                return Capture::pass;
            
            float x = local_mouse()[0] / scaleX + origoX;

            float min = std::numeric_limits<float>::max();

            int pos = 0;

            for(Point &i : points){
                if(std::abs(i.x - x) < min){
                    min = std::abs(i.x - x);
                    pos = (int)std::round(i.x);
                }
            }

            pos = (int)std::round(pos + offsetX);

            int length = clipEnd - clipBegin;
            clipBegin = pos;
            clipEnd = clipBegin + length;

            if(clipBegin != analyzer.clipBegin || clipEnd != analyzer.clipEnd){
                analyzer.clipBegin = clipBegin;
                analyzer.clipEnd = clipEnd;
                analyzer.save_clip();
                set_refresh();
            }
            return Capture::capture;
        }
    }

    return Capture::pass;
}

void AnalyzerGraph::on_refresh(){
    
    Graph::on_refresh();
    
    if(currentMode == regularMode && analyzer.loop && analyzer.clipping){
        
        float beginX = (clipBegin - origoX - offsetX) * scaleX;
        beginLine[0].position = sf::Vector2f(beginX, 0);
        beginLine[1].position = sf::Vector2f(beginX, canvasHeight);
        
        float endX = (clipEnd - origoX - offsetX) * scaleX;
        endLine[0].position = sf::Vector2f(endX, 0);
        endLine[1].position = sf::Vector2f(endX, canvasHeight);

        master->draw(beginLine);
        master->draw(endLine);
    }
}

void AnalyzerGraph::save(Saver &saver){

    saver.write_int((int)last);
    
    views[currentMode] = View{origoX, origoY, scaleX, scaleY};

    for(int mode = regularMode; mode < last; mode++){
        auto [oX, oY, sX, sY] = views[(Mode)mode];
        saver.write_float(oX);
        saver.write_float(oY);
        saver.write_float(sX);
        saver.write_float(sY);
    }

}

void AnalyzerGraph::load(Loader &loader){
    
    int modes = loader.read_int();

    for(int mode=regularMode; mode<modes; mode++){
        views[(Mode)mode] = {
            loader.read_float(),
            loader.read_float(),
            loader.read_float(),
            loader.read_float()
        };
    }
}

void AnalyzerGraph::set_view(Mode mode){


    if(mode == regularMode){
        set_unit_x("");
        set_unit_y("");
    } else if(mode == frequencyMode){
        set_unit_x("Hz");
        set_unit_y("");
    } else if(mode == peakMode){
        set_unit_x("");
        set_unit_y("");
    } else if(mode == correlationMode){
        set_unit_x("");
        set_unit_y("");
    }

    View view = views[mode];
    currentMode = mode;
    
    set_origo(view.origoX, view.origoY);
    set_scale(view.scaleX, view.scaleY);

    set_reconfig();
}

Analyzer::Analyzer(App *a) :
    Content(a),
    app(*a),
    linkId(Audio::generate_reciever_id()),
    slider(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    graph(*this, {.look = "agraph", .border = {0, 0, 0, 0}}),
    clipName(linkId),
    buttons(a, {.height = 20}),
    sourceNameBox(a, "source: (none)", {.look = "basetext", .border = {0, 0, 0, 1}}),
    
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

    buttons.put(0, 9, &sourceNameBox);
    sourceNameBox.text_stick(ui::Text::left);
    sourceNameBox.text_offset(1, -0.1);

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("link", [&](ui::Command c){ link_audio(c); });
    terminal.put_function("name", [&](ui::Command c){ set_name(c); });
    terminal.put_function("clip", [&](ui::Command c){ switch_clip(c); });
    terminal.put_function("play", [&](ui::Command c){ switch_play(c); });

    terminal.document("link", "[name] link audio to this analyzer");
    terminal.document("name", "[name] set clip name");
    terminal.document("clip", "start clipping. A portion of the source"
            "is automatically cached.");
    terminal.document("play", "switch source audio playback.");
}

Analyzer::~Analyzer(){
    if(source != nullptr){
        if(app.audioAlive) app.audio.detach_source(sourceName, linkId);
        delete source;
    }
    if(loop != nullptr) delete loop;
    if(player != nullptr) delete player;
}

std::string Analyzer::content_type(){ return type; }

const std::string Analyzer::type = "analyze";

int Analyzer::init_class = [](){
    Slot::add_content_type(type, [](App *a){ return new Analyzer(a); });
    return 0;
}();

void Analyzer::on_tick(){

    if(playing && player->getStatus() != wave::Player::Playing){
        playing = 0;
    }
    
    if(!sourceName.empty() && app.audio.pop_update(sourceName, linkId)){

        wave::Source *src = app.audio.get_source(sourceName, linkId);
    
        if(src){

            if(playing) player->lock();
            
            if(source != nullptr) delete source;
            source = src;

            loop->open(source);

            update_data();
            
            if(playing) player->unlock();
        }
    }
}

void Analyzer::save(Saver &saver){

    saver.write_string(sourceName);
    saver.write_int(position);
    saver.write_int(length);
    saver.write_int(dataMode);
    saver.write_string(clipName);

    graph.save(saver);
}

void Analyzer::load(Loader &loader){
    
    sourceName = loader.read_string();
    int pos = loader.read_int();
    length = loader.read_int();
    dataMode = (Mode)loader.read_int();
    clipName = loader.read_string();

    switch_mode(dataMode);

    link_audio({terminal, {sourceName}});

    position = pos;

    graph.load(loader);

    update_data();
    graph.set_view(dataMode);
}

void Analyzer::switch_mode(Mode mode){
    
    dataMode = mode;
    graph.set_view(mode);
    update_data();
}

void Analyzer::update_data(){

    if(source == nullptr){

        graph.set_data(std::vector<float>(length, 0.0f));

    } else {

        if(dataMode == regularMode){
            
            graph.set_data(loop->get(length, position));
            graph.set_offset_x(position);
            graph.set_scalar_x(1);
        
        } else if(dataMode == frequencyMode){
            
            auto data = math::fft(loop->get(length, position));
            data.resize(data.size()/2 + 1);

            graph.set_data(data);
            graph.set_offset_x(0);
            graph.set_scalar_x((double)loop->frameRate / length);
            
        } else if(dataMode == peakMode){

            graph.set_data(pitch.graph(loop->get(length, position)));
            graph.set_offset_x(0);
            graph.set_scalar_x(1);

        } else if(dataMode == correlationMode){
            
            graph.set_offset_x(0);
            graph.set_scalar_x(1);

        }
        
        graph.set_reconfig();
    }

}

void Analyzer::save_clip(){
    
    if(loop != nullptr){

        int length = clipEnd - clipBegin + 1;
        
        wave::Audio *audio = new wave::Audio();
        audio->name = clipName;
        audio->channels = loop->channels;
        audio->frameRate = loop->frameRate;
        audio->data = loop->get(length, clipBegin);

        app.audio.add_cache(audio);
    
    }
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

    if(playing) switch_play(c);

    wave::Source *src = app.audio.get_source(sourceName, linkId);
    
    if(src){
        
        if(source != nullptr) delete source;
        source = src;

        if(loop == nullptr) loop = new wave::Loop(source);
        else loop->open(source);

        sourceNameBox.set_text("source: " + sourceName);

        position = 0;

        update_data();
        switch_mode(dataMode);
    }
    else {
        c.source.push_error("audio source not found");
    }
}

void Analyzer::switch_play(ui::Command c){
    
    if(playing){
        playing = 0;
        player->stop();
    }
    else {

        if(source == nullptr){
            c.source.push_error("no audio linked");
        }
        else {
            
            playing = 1;

            if(player != nullptr) delete player;
            player = new wave::Player(loop);
            
            loop->seek(position);

            player->play();
        }
    }
}

void Analyzer::set_name(ui::Command c){
    
    std::string name = c.pop();

    if(name.empty()){
        c.source.push_error("name cannot be empty");
    }
    else {
        clipName = name;
    }

}

void Analyzer::switch_clip(ui::Command c){
    clipping ^= 1;
}

}
