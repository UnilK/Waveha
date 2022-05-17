#include "tools/analyzer.h"
#include "app/app.h"
#include "app/audio.h"
#include "app/creations.h"
#include "math/fft.h"
#include "math/ft.h"
#include "wstream/wstream.h"

#include <algorithm>
#include <math.h>
#include <limits>
#include <iostream>
#include <filesystem>

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
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)
                && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed = speed / 16 * analyzer.link.frameRate * analyzer.link.channels;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                speed /= 16;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                speed *= 16;

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
                
                int length = clipEnd - clipBegin;
                clipBegin = pos;
                clipEnd = clipBegin + length;
                
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
    terminal.document("*hotkeys", "use W and S to control interval length shown on graph\n"
            "A and D control interval position. shift and ctrl are speed modifiers\n"
            "Q moes the interval beginning to mouse position. E moves the end.\n"
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

        graph.set_data(change::peak_match_graph(link.get_loop(length, position), peakVars));
        graph.set_offset_x(0);
        graph.set_scalar_x(1);

    } else if(dataMode == correlationMode){
        
        graph.set_data(change::correlation_graph(link.get_loop(length, position), corrVars));
        graph.set_offset_x(0);
        graph.set_scalar_x(1);

    }
    
    graph.set_reconfig();
}

void Analyzer::save_clip(){
    
    if(link.size() > 0 && link.channels > 0 && link.frameRate > 0){

        int length = clipEnd - clipBegin + 1;
        
        wave::Audio *audio = new wave::Audio();
        audio->name = clipName;
        audio->channels = link.channels;
        audio->frameRate = link.frameRate;
        audio->data = link.get(length, clipBegin);

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
    message += "clip end: " + std::to_string(clipEnd);
    c.source.push_output(message);
}

void Analyzer::check_ml_data(ui::Command c){
    
    ml::TrainingData *all = app.creations.get_mldata(c.pop());
    if(all == nullptr) return;

    std::vector<std::complex<float> > freq;
    std::vector<float> rnd = (*all)[rand()%all->size()].first;
    freq.resize(rnd.size()/2);
    for(unsigned i=0; i<freq.size(); i++) freq[i] = {rnd[2*i], rnd[2*i+1]};
    graph.set_data(math::ift(freq, length));

    graph.set_reconfig();
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

}
