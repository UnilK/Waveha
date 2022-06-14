#include "tools/canvas.h"
#include "app/app.h"
#include "math/constants.h"
#include "app/audio.h"

#include <limits>
#include <cmath>
#include <iostream>

namespace app {

// canvas /////////////////////////////////////////////////////////////////////

Canvas::Canvas(CanvasTool &t, ui::Kwargs kwargs) :
    Graph((App*)t.get_master(), kwargs),
    tool(t),
    upLine(sf::LineStrip, 2),
    downLine(sf::LineStrip, 2)
{
    set_look(look);
    hasText = 0;
}

void Canvas::set_look(std::string look_){
    
    Graph::set_look(look_);
    
    indicatorSize = num("indicatorSize");
    
    for(int i=0; i<2; i++) upLine[i].color = color("axisColor");
    for(int i=0; i<2; i++) downLine[i].color = color("axisColor");
    
    brushT.setCharacterSize(indicatorSize);
    brushS.setCharacterSize(indicatorSize);
    brushM.setCharacterSize(indicatorSize);
    brushT.setFillColor(color("indicatorColor"));
    brushS.setFillColor(color("indicatorColor"));
    brushM.setFillColor(color("indicatorColor"));
    brushT.setPosition(5 + 2 * indicatorSize, 0);
    brushS.setPosition(5 + 2 * indicatorSize, indicatorSize);
    brushM.setPosition(5 + 2 * indicatorSize, 2 * indicatorSize);
    brushT.setFont(font("font"));
    brushS.setFont(font("font"));
    brushM.setFont(font("font"));

    update_info();
}

ui::Frame::Capture Canvas::on_event(sf::Event event, int priority){
    
    if(event.type == sf::Event::KeyPressed){
        
        if(event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D){
            
            int speed = 16;
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) speed /= 16;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) speed *= 16;

            tool.position = (tool.position + speed) % (int)tool.data.size();
            if(tool.position < 0) tool.position += tool.data.size();
            tool.update_data();

            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S){

            if(event.key.code == sf::Keyboard::S) tool.length = std::max(1, tool.length / 2);
            else tool.length = std::min(1<<18, tool.length * 2);

            tool.update_data();

            return Capture::capture;
        
        } else if(event.key.code == sf::Keyboard::U){

            tool.undo();
            return Capture::capture;

        } else if(event.key.code == sf::Keyboard::R){
            
            tool.redo();
            return Capture::capture;

        }
    }
    else if(event.type == sf::Event::MouseWheelScrolled){

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)){

            float scrollSpeed = 1.1f;
            if(event.mouseWheelScroll.delta < 0) scrollSpeed = 1 / scrollSpeed;

            tool.magnitude *= scrollSpeed;

            update_info();

            return Capture::capture;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)){
            
            float scrollSpeed = 1.1f;
            if(event.mouseWheelScroll.delta < 0) scrollSpeed = 1 / scrollSpeed;
       
            tool.brushSize *= scrollSpeed;
            
            update_info();
            
            return Capture::capture;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LAlt)){
            
            int i = 0;
            while(tool.brush != tool.brushes[i]) i++;

            int d = 1;
            if(event.mouseWheelScroll.delta < 0) d = -1;

            tool.brush = tool.brushes[(i+d+tool.brushes.size())%tool.brushes.size()];
            update_info();

            return Capture::capture;
        }
    }
    else if(event.type == sf::Event::MouseButtonPressed){
        
        if(event.mouseButton.button == sf::Mouse::Right){

            tool.newdo();

            auto [x, y] = local_mouse();

            xPos = x;
            yPos = canvasHeight - y;

            brushing = 1;
            return Capture::capture;
        }
    }
    else if(event.type == sf::Event::MouseButtonReleased){

        if(event.mouseButton.button == sf::Mouse::Right){
            
            brushing= 0;
            return Capture::capture;
        }

    }
    else if(event.type == sf::Event::MouseMoved){
        
        if(brushing){
            
            auto [x, y] = local_mouse();

            xPos = x;
            yPos = canvasHeight - y;
            
            return Capture::capture;
        }
    }

    return Graph::on_event(event, priority);
}

void Canvas::on_refresh(){

    Graph::on_refresh();
    
    double upY = canvasHeight + (origoY + 1) * scaleY;
    double downY = canvasHeight + (origoY - 1) * scaleY;

    upLine[0].position = sf::Vector2f(0, upY);
    upLine[1].position = sf::Vector2f(canvasWidth, upY);
    
    downLine[0].position = sf::Vector2f(0, downY);
    downLine[1].position = sf::Vector2f(canvasWidth, downY);
    
    master->draw(upLine);
    master->draw(downLine);
    master->draw(brushT);
    master->draw(brushS);
    master->draw(brushM);
}

void Canvas::on_tick(){
    
    if(brushing){

        double xx = origoX + xPos / scaleX + tool.position;
        double yy = origoY + yPos / scaleY;
        
        int left = (int)std::floor(xx) % tool.data.size();
        if(left < 0) left += tool.data.size();
        int right = (left + 1) % tool.data.size();
        
        double d = (xx - std::floor(xx));
        double dy = yy - ((1-d)*tool.data[left]+d*tool.data[right]);

        tool.draw(left + d, dy);
    }
}

void Canvas::update_info(){

    brushT.setString("Brush: " + tool.brush);
    brushS.setString("Size: " + std::to_string(tool.brushSize));
    brushM.setString("Magnitude: " + std::to_string(tool.magnitude));

    set_refresh();
}

// canvastool /////////////////////////////////////////////////////////////////

CanvasTool::CanvasTool(App *a) :
    Content(a),
    app(*a),
    data(128, 0.0f),
    slider(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    canvas(*this, {.look = "agraph", .border = {0, 0, 0, 0}})
{
    setup_grid(2, 1);
    fill_width({1});
    fill_height({1, 0});

    put(0, 0, &canvas);
    put(1, 0, &slider);
    
    slider.stack.create(1);
    slider.stack.put(&terminal, 0);
    slider.stack.update();
    slider.stack.fill_height({1, 0});
    slider.stack.set_border(0, 0, 0, 0);
    
    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("name", [&](ui::Command c){ name_output(c); });
    terminal.put_function("size", [&](ui::Command c){ set_size(c); });
    terminal.put_function("c", [&](ui::Command c){ config_brush(c); });
    terminal.put_function("nil", [&](ui::Command c){ nil_data(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });
    
    terminal.document("name", "[name] set output name");
    terminal.document("size", "[size] set interval size");
    terminal.document("c", "[arg] [value] set brush config value");
    terminal.document("nil", "sets all data values to zero");
    terminal.document("info", "list tool info");
    terminal.document("*hotkeys",
            "W and S control shown interval size\n"
            "A and D control interval position. shift and ctrl are speed modifiers\n"
            "hold LMB to move graph around and scroll to zoom\n"
            "hold RMB to use brush on canvas\n"
            "U to undo, R to redo\n"
            "alt + scroll to change brush type\n"
            "shift + scroll to change brush size\n"
            "ctrl + scroll to chage brush magnitude\n");

    update_data();
    canvas.fit_x();
    canvas.set_origo(0, -1);
    canvas.scale(1, 100);

    canvas.update_info();
}

CanvasTool::~CanvasTool(){}

namespace Factory { extern std::string canvas; }
std::string CanvasTool::content_type(){ return Factory::canvas; }

void CanvasTool::save(ui::Saver &saver){
    saver.write_string(output);
    saver.write_string(brush);
    saver.write_float(phase);
    saver.write_float(brushSize);
    saver.write_float(magnitude);
    saver.write_unsigned(frequency);
    saver.write_unsigned(data.size());
    for(float i : data) saver.write_float(i);
    canvas.save(saver);
}

void CanvasTool::load(ui::Loader &loader){
    output = loader.read_string();
    brush = loader.read_string();
    phase = loader.read_float();
    brushSize = loader.read_float();
    magnitude = loader.read_float();
    frequency = loader.read_unsigned();
    data.resize(loader.read_unsigned());
    for(float &i : data) i = loader.read_float();
    canvas.load(loader);
    update_data();
    canvas.update_info();
}

void CanvasTool::name_output(ui::Command c){
    
    std::string newName = c.pop();
    if(!newName.empty()){
        output = newName;
        update_data();
    } else {
        c.source.push_error("output name cannot be empty");
    }

}

void CanvasTool::set_size(ui::Command c){
    
    try {

        unsigned size = std::stoul(c.pop());
        if(size >= MIN_SIZE){
            newdo();
            data.resize(size, 0.0f);
            update_data();
        } else {
            c.source.push_error("interval size is too small");
        }
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void CanvasTool::config_brush(ui::Command c){

    try {
    
        std::string type = c.pop();

        if(type == "phase"){
            phase = std::stof(c.pop())*2*PIF;
        } else if(type == "mag"){
            magnitude = std::stof(c.pop());
        } else if(type == "size"){
            brushSize = std::stof(c.pop());
        } else if(type == "freq"){
            frequency = std::stoul(c.pop());
        } else {
            c.source.push_error("type \""+type+"\" not in [phase, mag, size]");
        }

    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void CanvasTool::info(ui::Command c){

    std::string message;
    message += "output: " + output + "\n";
    message += "phase: " + std::to_string(std::arg(phase)) + "\n";
    message += "interval size: " + std::to_string(data.size()) + "\n";
    message += "brush size: " + std::to_string(brushSize) + "\n";
    message += "magnitude: " + std::to_string(magnitude);
    c.source.push_output(message);
}

void CanvasTool::nil_data(ui::Command c){
    newdo();
    for(float &i : data) i = 0.0f;
    update_data();
}

void CanvasTool::draw(double x, double y){

    if(brush == "packet"){
        
        double l = x - brushSize;
        double r = x + brushSize;

        int left = std::ceil(l);
        int right = std::floor(r);

        int size = data.size();

        for(int i=left, j=(left%size+size)%size; i<=right; i++, j=(j+1)%size){
            data[j] += magnitude * y * (0.5f - 0.5f * std::cos((i-l) * PIF / brushSize))
                * std::cos(2*PIF*j*frequency / size + phase);
        }
    }
    else if(brush == "damp"){

        double l = x - brushSize;
        double r = x + brushSize;

        int left = std::ceil(l);
        int right = std::floor(r);

        int size = data.size();

        for(int i=left, j=(left%size+size)%size; i<=right; i++, j=(j+1)%size){
            data[j] *= std::exp(-magnitude * (0.5f - 0.5f * std::cos((i-l) * PIF / brushSize)));
        }
    }

    update_data();
}

void CanvasTool::update_data(){

    std::vector<float> show(length);
    for(int i=0; i<length; i++) show[i] = data[(i+position)%data.size()];

    canvas.set_data(show);

    wave::Audio *audio = new wave::Audio();
    audio->name = output;
    audio->channels = 1;
    audio->frameRate = 44100;
    audio->data = data;

    app.audio.add_cache(audio);
}

void CanvasTool::newdo(){
    redoh.clear();
    undoh.push_back(data);
    handle_history();
    update_data();
}

void CanvasTool::undo(){
    if(undoh.size()){
        redoh.push_back(data);
        data = undoh.back();
        undoh.pop_back();
        update_data();
    }
}

void CanvasTool::redo(){
    if(redoh.size()){
        undoh.push_back(data);
        data = redoh.back();
        redoh.pop_back();
        update_data();
    }
}

void CanvasTool::handle_history(){
    if(undoh.size() > HISTORY_SIZE){
        for(unsigned i=0; i+32 < undoh.size(); i++) undoh[i] = undoh[i+32];
        undoh.resize(undoh.size() - 32);
    }
}

}

