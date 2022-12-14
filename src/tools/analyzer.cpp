#include "tools/analyzer.h"
#include "app/app.h"
#include "app/audio.h"
#include "app/creations.h"
#include "math/fft.h"
#include "math/ft.h"
#include "wstream/wstream.h"
#include "change/detector.h"
#include "ml/waves.h"
#include "math/constants.h"
#include "change/changer1.h"
#include "change/changer2.h"
#include "change/changer3.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>
#include <filesystem>
#include <cassert>
#include <random>

namespace app {

// graph //////////////////////////////////////////////////////////////////////

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
            
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed = speed / 16 * analyzer.link.frameRate * analyzer.link.channels;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt))
                speed = 128;
            
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(analyzer.link.size() != 0){
                analyzer.position = (analyzer.position + speed) % (int)analyzer.link.size();
                if(analyzer.position < 0) analyzer.position += analyzer.link.size();
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
        
        } else if(event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::E){
            
            if(currentMode != regularMode) return Capture::pass;
            
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
            
            if(analyzer.link.size() == 0) return Capture::capture;

            if(event.key.code == sf::Keyboard::Q){
                
                analyzer.position = pos % (int)analyzer.link.size();
                analyzer.update_data();
            
            } else {

                analyzer.position = (pos - analyzer.length) % (int)analyzer.link.size();
                if(analyzer.position < 0) analyzer.position += analyzer.link.size();
                analyzer.update_data();
            }

            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::I || event.key.code == sf::Keyboard::K){
           
            if(currentMode != regularMode || (!analyzer.clipping && !analyzer.datasetting)
                    || analyzer.link.size() == 0) return Capture::pass;

            int speed = 16;
            if(event.key.code == sf::Keyboard::K) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;

            clipEnd = (clipEnd + speed) % analyzer.link.size();
            if(clipEnd < clipBegin) clipEnd += analyzer.link.size();

            if(clipBegin != analyzer.clipBegin || clipEnd != analyzer.clipEnd){
                analyzer.clipBegin = clipBegin;
                analyzer.clipEnd = clipEnd;
                analyzer.save_clip();
                set_refresh();
            }
            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::J || event.key.code == sf::Keyboard::L){
            
            if(currentMode != regularMode || (!analyzer.clipping && !analyzer.datasetting)
                    || analyzer.link.size() == 0) return Capture::pass;
            
            int speed = 16;
            if(event.key.code == sf::Keyboard::J) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;

            int length = clipEnd - clipBegin;

            clipBegin = (clipBegin + speed) % (int)analyzer.link.size();
            if(clipBegin < 0) clipBegin += analyzer.link.size();
            
            clipEnd = clipBegin + length;
            
            if(clipBegin != analyzer.clipBegin || clipEnd != analyzer.clipEnd){
                analyzer.clipBegin = clipBegin;
                analyzer.clipEnd = clipEnd;
                if(analyzer.clipping) analyzer.save_clip();
                set_refresh();
            }
            return Capture::capture;
        
        } else if(event.key.code == sf::Keyboard::U || event.key.code == sf::Keyboard::O){
            
            if(currentMode != regularMode ||
                    (!analyzer.clipping && !analyzer.datasetting)) return Capture::pass;
            
            float x = local_mouse()[0] / scaleX + origoX;

            float min = std::numeric_limits<float>::max();

            int pos = 0;

            for(Point &i : points){
                if(std::abs(i.x - x) < min){
                    min = std::abs(i.x - x);
                    pos = (int)std::round(i.x);
                }
            }

            pos = (int)std::round(pos + offsetX) % (int)analyzer.link.size();
            
            if(event.key.code == sf::Keyboard::U){
                int length = clipEnd - clipBegin;
                clipBegin = pos;
                clipEnd = clipBegin + length;
            } else {
                clipEnd = pos;
                if(clipEnd < clipBegin) clipEnd += analyzer.link.size();
            }

            if(clipBegin != analyzer.clipBegin || clipEnd != analyzer.clipEnd){
                analyzer.clipBegin = clipBegin;
                analyzer.clipEnd = clipEnd;
                if(analyzer.clipping) analyzer.save_clip();
                set_refresh();
            }
            return Capture::capture;
        }
        else if(event.key.code == sf::Keyboard::C){
            analyzer.switch_clip({analyzer.terminal, {}});
            set_refresh();
            return Capture::capture;
        }
        else if(event.key.code == sf::Keyboard::V){
            analyzer.switch_dataset({analyzer.terminal, {}});
            set_refresh();
            return Capture::capture;
        }
        else if(event.key.code == sf::Keyboard::F){
            analyzer.clip_to_dataset({analyzer.terminal, {}});
            return Capture::capture;
        }
        else if(event.key.code == sf::Keyboard::P){
            analyzer.switch_play({analyzer.terminal, {}});
            return Capture::capture;
        }

    }

    return Capture::pass;
}

void AnalyzerGraph::on_refresh(){
    
    Graph::on_refresh();
    
    if(currentMode == regularMode && (analyzer.clipping || analyzer.datasetting)){
        
        double xoff = offsetX;
        if(xoff > (double)clipEnd)
            xoff -= analyzer.link.size();
        if((double)clipBegin - xoff >= (double)analyzer.link.size())
            xoff += analyzer.link.size();
        if(clipEnd > (int)analyzer.link.size() && xoff + analyzer.link.size() < (double)clipEnd)
            xoff += analyzer.link.size();

        double beginX = (clipBegin - xoff - origoX) * scaleX;
        double endX = (clipEnd - xoff - origoX) * scaleX;
        
        beginLine[0].position = sf::Vector2f(beginX, 0);
        beginLine[1].position = sf::Vector2f(beginX, canvasHeight);
        
        endLine[0].position = sf::Vector2f(endX, 0);
        endLine[1].position = sf::Vector2f(endX, canvasHeight);

        master->draw(beginLine);
        master->draw(endLine);
    }
}

void AnalyzerGraph::save(ui::Saver &saver){

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

void AnalyzerGraph::load(ui::Loader &loader){

    int modes = loader.read_int();

    for(int mode=regularMode; mode<modes; mode++){
        views[(Mode)mode] = {
            loader.read_float(),
            loader.read_float(),
            loader.read_float(),
            loader.read_float()
        };
    }

    clipBegin = analyzer.clipBegin;
    clipEnd = analyzer.clipEnd;
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

// analyzer ///////////////////////////////////////////////////////////////////

Analyzer::Analyzer(App *a) :
    Content(a),
    app(*a),
    link(*a),
    player(&link),
    clipName("untitled"),
    slider(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    graph(*this, {.look = "agraph", .border = {0, 0, 0, 0}}),
    buttons(a, {.height = 20}),
    sourceNameBox(a, "source: (none)", {.look = "basetext", .border = {0, 0, 0, 1}}),
    
    switchRegular(
            a,
            [&](){ switch_mode(regularMode); },
            "time",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}}),
    
    switchFrequency(
            a,
            [&](){ switch_mode(frequencyMode); },
            "freq",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}}),
    
    switchPeak(
            a,
            [&](){ switch_mode(peakMode); },
            "peak",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}}),
    
    switchCorrelation(
            a,
            [&](){ switch_mode(correlationMode); },
            "corr",
            {.look = "basebutton", .width = 50, .border = {0, 1, 0, 1}})
{

    player.set_loop(1);

    setup_grid(2, 1);
    fill_width({1});
    fill_height({1, 0});

    put(0, 0, &graph);
    put(1, 0, &slider);
    slider.set_scrollable(0);

    slider.stack.create(2);
    slider.stack.put(&buttons, 0);
    slider.stack.put(&terminal, 1);
    slider.stack.update();
    slider.stack.fill_height({0, 1, 0});
    slider.stack.set_border(0, 0, 0, 0);
    slider.functions.set_border(0, 0, 1, 1);

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

    terminal.put_directory("au", &app.audio.dir);
    
    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("link", [&](ui::Command c){ link_audio(c); });
    terminal.put_function("cname", [&](ui::Command c){ name_clip(c); });
    terminal.put_function("clip", [&](ui::Command c){ switch_clip(c); });
    terminal.put_function("play", [&](ui::Command c){ switch_play(c); });
    terminal.put_function("speak", [&](ui::Command c){ setup_peaks(c); });
    terminal.put_function("scorr", [&](ui::Command c){ setup_correlation(c); });
    terminal.put_function("dname", [&](ui::Command c){ name_dataset(c); });
    terminal.put_function("dataset", [&](ui::Command c){ switch_dataset(c); });
    terminal.put_function("reverb", [&](ui::Command c){ reverb_audio(c); });
    terminal.put_function("cfeed", [&](ui::Command c){ config_feedback(c); });
    terminal.put_function("ppitch", [&](ui::Command c){ process_pitch(c); });
    terminal.put_function("translate", [&](ui::Command c){ translate_pitch(c); });
    terminal.put_function("inter", [&](ui::Command c){ set_interpolate(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });
    
    terminal.put_function("check", [&](ui::Command c){ check_ml_data(c); });

    terminal.document("link", "[name] link audio to this analyzer");
    terminal.document("cname", "[name] set clip name");
    terminal.document("dname", "[name] set dataset directory");
    terminal.document("dataset", "switch datasetting. Pressing F on graph will save the clip to"
            " the dataset directory.");
    terminal.document("clip", "start clipping. A portion of the source"
            " is automatically cached.");
    terminal.document("play", "switch source audio playback.");
    terminal.document("speak", "[variable] [value] set value to peak match variable.");
    terminal.document("scorr", "[variable] [value] set value to correlation variable.");
    terminal.document("check", "[data] check wave training data");
    terminal.document("reverb", "[input] [output] reverb process input audio and save it to output");
    terminal.document("cfeed", "[delay] [magnitude] add reverb feedback");
    terminal.document("ppitch", "[audio] [output] get pitch envelopoe of audio");
    terminal.document("traslate", "[audio] [output] [factor] translate the pitch of audio");
    terminal.document("inter", "interpolate edges when clipping");
    terminal.document("*hotkeys",
            "use W and S to control interval length shown on graph\n"
            "A and D control interval position. shift and ctrl are speed modifiers\n"
            "Q moves the interval beginning to mouse position. E moves the end.\n"
            "move graph inspector with mouse\n"
            "hold LMB to move graph around and scroll to zoom. shift and control lock axis\n"
            "C to switch clipping. U to move begin to mouse position, O to move end\n"
            "I J K L correspond to W A S D but for the clip\n"
            "V to switch dataset collection. F to collect a dataset sample (clip)\n"
            "P to switch audio playback");
    terminal.document("info", "list the configuration");
    
}

Analyzer::~Analyzer(){}

namespace Factory { extern std::string analyzer; }
std::string Analyzer::content_type(){ return Factory::analyzer; }

void Analyzer::on_tick(){
    if(playing && player.getStatus() != wave::Player::Playing) playing = 0;
    if(link.pop_update()) update_data();
}

void Analyzer::save(ui::Saver &saver){

    saver.write_string(link.source());
    saver.write_int(position);
    saver.write_int(length);
    saver.write_int(dataMode);
    saver.write_string(clipName);
    saver.write_string(dataset);
    saver.write_int(clipBegin);
    saver.write_int(clipEnd);

    saver.write_unsigned(feedback.size());
    for(auto [delay, mag] : feedback){
        saver.write_int(delay);
        saver.write_float(mag);
    }

    saver.write_float(slider.targetHeight);

    graph.save(saver);
}

void Analyzer::load(ui::Loader &loader){
    
    std::string sourceName = loader.read_string();
    int pos = loader.read_int();
    length = loader.read_int();
    dataMode = (Mode)loader.read_int();
    clipName = loader.read_string();
    dataset = loader.read_string();
    clipBegin = loader.read_int();
    clipEnd = loader.read_int();

    switch_mode(dataMode);

    link_audio({terminal, {sourceName}});

    position = pos;

    feedback.clear();
    unsigned feedbacks = loader.read_unsigned();
    for(unsigned i=0; i<feedbacks; i++)
        feedback[loader.read_int()] = loader.read_float();

    slider.set_target_size(0, loader.read_float());
    
    graph.load(loader);
    
    terminal.clear({terminal, {}});

    update_data();
    graph.set_view(dataMode);
}

void Analyzer::switch_mode(Mode mode){
    
    dataMode = mode;
    graph.set_view(mode);
    update_data();
}

void Analyzer::update_data(){

    if(dataMode == regularMode){
        
        graph.set_data(link.get_loop(length, position));
        graph.set_offset_x(position);
        graph.set_scalar_x(1);
    
    } else if(dataMode == frequencyMode){
        
        auto data = math::fft(link.get_loop(length, position));
        data.resize(data.size()/2 + 1);

        graph.set_data(data);
        graph.set_offset_x(0);
        graph.set_scalar_x((double)link.frameRate / length);
        
    } else if(dataMode == peakMode){

        graph.set_data(change::phase_graph(link.get_loop(link.size(), position), link.size()));
        
        // graph.set_data(change::phase_graph(link.get_loop(length, position), link.size()));
        graph.set_offset_x(0);
        graph.set_scalar_x(1);

    } else if(dataMode == correlationMode){
        
        int speed = 16;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            speed /= 16;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            speed *= 16;
        
        graph.set_data(change::correlation_graph(link.get_loop(speed, position+length-speed), corrVars));
        graph.set_offset_x(0);
        graph.set_scalar_x(1);

    }
    
    graph.set_reconfig();
}

void Analyzer::save_clip(){
    
    if(link.size() > 0 && link.channels > 0 && link.frameRate > 0){

        int length = clipEnd - clipBegin + 1;
        
        auto data = link.get(length, clipBegin);

        if(interpolate && data.size() > 20){
            
            int n = data.size();
            int d = n * 0.1;

            std::vector<float> v, l, m, r;
            for(int i=-d+1; i<0; i++) v.push_back(data[n+i]-data[n+i-1]);
            v.push_back(data[0]-data[n-1]);
            for(int i=1; i<d; i++) v.push_back(data[i]-data[i-1]);

            int z = v.size();
            l.resize(z, 0.0f);
            m.resize(z, 0.0f);
            r.resize(z, 0.0f);

            float a = 0.33f, b = std::pow(1e-2f, 1.0f / (d-1));

            l[d-1] = r[d-1] = a;
            m[d-1] = 1.0f - 2 * a;
            for(int i=1; i<d; i++){
                r[d-1-i] = l[d-1+i] = l[d-i];
                l[d-1-i] = r[d-1+i] = b * r[d-1-i];
                m[d-1-i] = m[d-1+i] = 1.0f - l[d-i-1] - r[d-i-1];
            }
            m[0] = m[z-1] = m[0] - l[0];
            l[0] = r[z-1] = 0.0f;

            int cnt = 0;
            for(int i=0; i<10; i++){
                std::vector<float> w(z, 0.0f);
                for(int i=1; i+1<z; i++){
                    w[i] = v[i] * m[i] + v[i-1] * r[i-1] + v[i+1] * l[i+1];
                }
                w[0] = v[0] * m[0] + v[1] * l[1];
                w[z-1] = v[z-1] * m[z-1] + v[z-2] * r[z-2];
                v = w;
                cnt++;
            }

            for(int i=-d+1; i<0; i++) data[n+i] = data[n+i-1] + v[d-1+i];
            data[0] = data[n-1] + v[d-1];
            for(int i=1; i<d; i++) data[i] = data[i-1] + v[d-1+i];

        }

        wave::Audio *audio = new wave::Audio();
        audio->name = clipName;
        audio->channels = link.channels;
        audio->frameRate = link.frameRate;
        audio->data = data;

        app.audio.add_cache(audio);
    
    }
}

void Analyzer::link_audio(ui::Command c){
    
    std::string source = c.pop();

    if(playing) switch_play(c);

    if(!source.empty()){
        
        link.open(source);
        player.open(&link);

        sourceNameBox.set_text("source: " + source);

        position = 0;

        update_data();
        switch_mode(dataMode);
    }
    else {
        c.source.push_error("give a source");
    }
}

void Analyzer::switch_play(ui::Command c){
    
    if(playing){
        playing = 0;
        player.stop();
    }
    else {
        playing = 1;
        link.seek(position);
        player.play();
    }
}

void Analyzer::name_clip(ui::Command c){
    
    std::string name = c.pop();

    if(name.empty()) c.source.push_error("clip name cannot be empty");
    else clipName = name;
}

void Analyzer::switch_clip(ui::Command c){
    if(!clipping){
        datasetting = 0;
        clipping = 1;
    } else {
        clipping = 0;
    }
}

void Analyzer::setup_peaks(ui::Command c){
    
    auto var = c.pop();

    try {
        if(var == "peaks"){
            peakVars.peaks = std::stoi(c.pop());
            update_data();
        }
        else if(var == "exp"){
            peakVars.exponent = std::stof(c.pop());
            update_data();
        }
        else {
            c.source.push_error(var + " not in {peaks, exp}");
        }
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }

}

void Analyzer::setup_correlation(ui::Command c){

    auto var = c.pop();
    
    try {
        if(var == "exp"){
            corrVars.exponent = std::stof(c.pop());
            update_data();
        }
        else if(var == "sign"){
            corrVars.sign = std::stoi(c.pop());
            update_data();
        }
        else {
            c.source.push_error(var + " not in {exp, sign}");
        }
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Analyzer::info(ui::Command c){
    
    std::string message;
    message += "source: " + link.source() + "\n";
    message += "view position: " + std::to_string(position) + "\n";
    message += "view length: " + std::to_string(length) + "\n";
    message += "mode: " + std::to_string(dataMode) + "\n";
    message += "clip name: " + clipName + "\n";
    message += "dataset name: " + dataset + "\n";
    message += "clip begin: " + std::to_string(clipBegin) + "\n";
    message += "clip end: " + std::to_string(clipEnd) + "\n";
    message += "reverb: ";
    for(auto [delay, mag] : feedback)
        message += "("+std::to_string(delay)+","+std::to_string(mag)+"), ";
    c.source.push_output(message);
}

void Analyzer::check_ml_data(ui::Command c){
    
    std::string name = c.pop();
    auto type = app.creations.data_type(name);

    if(type != "waver" && type != "wavel" && type != "wavem" && type != "wave") return;

    ml::TrainingData *all = app.creations.get_mldata(name);
    if(all == nullptr) return;

    std::vector<std::complex<float> > freq;
    auto [rnd, label] = all->get_random();
    
    freq.resize(rnd.size()/2 - 1);
    for(unsigned i=0; i<freq.size(); i++) freq[i] = {rnd[2*i], rnd[2*i+1]};
    
    auto waves = math::ift(freq, length);
    for(int i=0; i<length; i++) waves.push_back(waves[i]);
    graph.set_data(waves);

    graph.set_reconfig();

    std::vector<std::string> phonetics = {
        "a","e","o","u","w","y","ae","oe","","","","","","","","",
        "i","j","g","v","","","","","","","","","","","","",
        "m","n","ng","th","","","","","","","","","","","","",
        "l","r","","","","","","","","","","","","","",""};
    
    std::string message = "label: ";
    for(size_t i=0; i<phonetics.size(); i++){
        if(label[i] > 0.5f) message += phonetics[i] + ",";
    }

    c.source.push_output(message);
}

void Analyzer::name_dataset(ui::Command c){
    
    std::string name = c.pop();

    if(name.empty()) c.source.push_error("dataset directory name cannot be empty");
    else dataset = name;
}

void Analyzer::switch_dataset(ui::Command c){

    if(!datasetting){
        datasetting = 1;
        clipping = 0;
    } else {
        datasetting = 0;
    }
}

void Analyzer::clip_to_dataset(ui::Command c){

    if(!datasetting){
        c.source.push_error("switch on datasetting to save clips");
        return;
    }
    else if(!std::filesystem::exists(std::filesystem::path(dataset))){
        c.source.push_error("dataset directory \""+ dataset +"\" does not exist");
        return;
    }

    auto iswave = [](std::string &s) -> bool {
        if(s.size() < 4) return 0;
        return s.substr(s.size()-4) == ".wav";
    };

    std::vector<int> numbers;
    for(auto &file : std::filesystem::directory_iterator(dataset)){
        
        std::string f = file.path();
        if(!iswave(f)) continue;

        try {
            int index = f.size()-5, length = 0;
            while(index >= 0 && f[index] >= '0' && f[index] <= '9'){
                index--;
                length++;
            } index++;
            numbers.push_back(std::stoi(f.substr(index, length)));
        }
        catch (const std::invalid_argument &e){
            continue;
        }
    }

    int max = 0;
    std::sort(numbers.begin(), numbers.end());
    numbers.resize(std::unique(numbers.begin(), numbers.end()) - numbers.begin());

    for(int i : numbers) if(i == max) max++;

    std::string filename = dataset + "/" + std::to_string(max) + ".wav";

    owstream file(filename, 1, 1, 16, 44100);
    file.write_file(link.get(clipEnd + 1 - clipBegin, clipBegin));

    c.source.push_output("file created: " + filename);

}

void Analyzer::reverb_audio(ui::Command c){
    
    std::string in = c.pop(), out = c.pop();

    if(in.empty() || out.empty()){
        c.source.push_error("give input and output audio names");
        return;
    }

    wave::Audio *audio = new wave::Audio();
    audio->name = out;
    audio->data = app.audio.get_audio(in, app.audio.audio_size(in), 0);

    for(int i=0; i<(int)audio->data.size(); i++){
        for(auto &[delay, mag] : feedback){
            if(delay <= i) audio->data[i] += audio->data[i - delay] * mag;
        }
    }

    app.audio.add_cache(audio);
}

void Analyzer::config_feedback(ui::Command c){

    try {
        auto probe = c.pop();
        
        if(probe == "."){
            feedback.clear();
            return;
        }

        int delay = std::stoul(probe);
        float mag = std::stof(c.pop());
        feedback[delay] = mag;
        if(mag == 0.0f) feedback.erase(delay);
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void Analyzer::process_pitch(ui::Command c){
    
    std::string input = c.pop(), output = c.pop();

    if(input.empty() || output.empty()){
        c.source.push_error("give an audio source and an output name");
        return;
    }

    AudioLink src(app);
    src.open(input);

    // change::Detector detector;
    change::Changer2 detector;
    
    std::vector<float> pitch, temp;

    unsigned window = 32;

    if(src.size() == 0) c.source.push_error("input audio does not exist");

    while(src.good){

        /*
        src.pull(window, temp);
        detector.feed(temp);

        if(detector.voiced) pitch.push_back(detector.pitch);
        else pitch.push_back(0.0f);
        */
        
        src.pull(window, temp);
        for(float i : temp) detector.process(i);
        
        pitch.push_back(detector.get_pitch());
    }

    wave::Audio *out = new wave::Audio();
    out->name = output;
    out->data = pitch;
    app.audio.add_cache(out);
}

void Analyzer::translate_pitch(ui::Command c){
  
    std::string input = c.pop(), output = c.pop();

    if(input.empty() || output.empty()){
        c.source.push_error("give an audio source and an output name");
        return;
    }
    
    AudioLink src(app);
    src.open(input);

    change::Changer2 changer;
    
    const int N = 32;

    std::vector<float> result, tmp;

    while(src.good){
        src.pull(N, tmp);
        for(float i : tmp) result.push_back(changer.process(i));
    }

    wave::Audio *out = new wave::Audio();
    out->name = output;
    out->data = result;
    app.audio.add_cache(out);

    /*
    const int N = 100;
    const int n = 128;
    auto freq = math::ft(src.get(src.size(), 0), n);
    int m = N * src.size();

    std::mt19937 rgen;
    rgen.seed(0);

    std::normal_distribution<float> distr(0.0f, 0.7f);

    auto rnd = [&]() -> float { return std::uniform_real_distribution<float>(0, 1)(rgen); };
    auto norm = [&]() -> float { return distr(rgen); };

    std::vector<double> phase(n, 0.0f), abs(n, 0.0f), spd(n, 0.0f), anchor(n, 0.0f);
    std::vector<float> result(m, 0.0f);

    float fact = 2.0f * M_PI / src.size();
    
    for(int i=0; i<n; i++){
        abs[i] = std::abs(freq[i]);
        if(abs[i] != 0.0f) phase[i] = std::arg(freq[i]);
        anchor[i] = spd[i] = fact * (i+1);
        phase[i] = 2.0f * M_PI * rnd();
    }
    
    for(int i=0; i<100; i++){
        for(int j=0; j<n; j++){
            spd[j] += fact * (norm() + (anchor[j] - spd[j]) * 0.8f);
        }
    }
    
    for(int i=0; i<m; i++){
        for(int j=0; j<n; j++){
            result[i] += std::polar(abs[j], phase[j]).real();
            phase[j] += spd[j];
            spd[j] += fact * ((rnd() - 0.5f) + (anchor[j] - spd[j]) * 0.8f);
            if(phase[j] > M_PI) phase[j] -= 2 * M_PI;
        }
    }
    */
}
    
void Analyzer::set_interpolate(ui::Command c){
    interpolate ^= 1;
    update_data();
}

}
