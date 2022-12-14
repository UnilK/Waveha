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
    downLine(sf::LineStrip, 2),
    sideLine(sf::LineStrip, 2)
{
    set_look(look);
    hasText = 0;
    hasInspector = 0;
}

void Canvas::set_look(std::string look_){
    
    Graph::set_look(look_);
   
    indicatorSize = num("indicatorSize");
    
    for(int i=0; i<2; i++) upLine[i].color = color("axisColor");
    for(int i=0; i<2; i++) downLine[i].color = color("axisColor");
    for(int i=0; i<2; i++) sideLine[i].color = color("axisColor");
    
    for(sf::Text *i : {&brushT, &brushS, &brushM, &cursorX, &cursorY}){
        i->setCharacterSize(indicatorSize);
        i->setFillColor(color("indicatorColor"));
        i->setFont(font("font"));
    }

    brushT.setPosition(5 + 2 * indicatorSize, 0 * indicatorSize);
    brushS.setPosition(5 + 2 * indicatorSize, 1 * indicatorSize);
    brushM.setPosition(5 + 2 * indicatorSize, 2 * indicatorSize);
    cursorX.setPosition(5 + 2 * indicatorSize, 3 * indicatorSize);
    cursorY.setPosition(5 + 2 * indicatorSize, 4 * indicatorSize);

    update_info();
}

ui::Frame::Capture Canvas::on_event(sf::Event event, int priority){
    
    if(event.type == sf::Event::KeyPressed){
        
        if((event.key.code == sf::Keyboard::A || event.key.code == sf::Keyboard::D)
                && tool.cyclic){
            
            int speed = 16;
            if(event.key.code == sf::Keyboard::A) speed = -speed;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) speed /= 16;
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) speed *= 16;

            tool.position = (tool.position + speed) % (int)tool.data.size();
            if(tool.position < 0) tool.position += tool.data.size();
            tool.update_data();

            return Capture::capture;

        } else if((event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::S)
                && tool.cyclic){

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

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F)){

            float scrollSpeed = 1.1f;
            if(event.mouseWheelScroll.delta < 0) scrollSpeed = 1 / scrollSpeed;

            tool.magnitude *= scrollSpeed;

            update_info();

            return Capture::capture;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::G)){
            
            float scrollSpeed = 1.1f;
            if(event.mouseWheelScroll.delta < 0) scrollSpeed = 1 / scrollSpeed;
       
            tool.brushSize *= scrollSpeed;
            
            update_info();
            
            return Capture::capture;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::H)){
            
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
        
        auto [x, y] = local_mouse();

        double xx = origoX + x / scaleX + tool.position;
        double yy = origoY + (canvasHeight - y) / scaleY;
        cursorX.setString("X: " + std::to_string(xx));
        cursorY.setString("Y: " + std::to_string(yy));
        
        if(brushing){
            
            xPos = x;
            yPos = canvasHeight - y;
            
            return Capture::capture;
        }
    }

    return Graph::on_event(event, priority);
}

void Canvas::on_refresh(){

    Graph::on_refresh();
    
    double upY = canvasHeight + (origoY - upHeight) * scaleY;
    double downY = canvasHeight + (origoY + downHeight) * scaleY;
    
    double rightX = (tool.data.size() - 1 - origoX) * scaleX;

    upLine[0].position = sf::Vector2f(0, upY);
    upLine[1].position = sf::Vector2f(canvasWidth, upY);
    
    downLine[0].position = sf::Vector2f(0, downY);
    downLine[1].position = sf::Vector2f(canvasWidth, downY);
    
    sideLine[0].position = sf::Vector2f(rightX, 0);
    sideLine[1].position = sf::Vector2f(rightX, canvasHeight);
    
    master->draw(upLine);
    master->draw(downLine);
    master->draw(sideLine);
    
    for(sf::Text *i : {&brushT, &brushS, &brushM, &cursorX, &cursorY}){
        master->draw(*i);
    }
}

void Canvas::on_tick(){
    
    if(brushing){

        double xx = origoX + xPos / scaleX + tool.position;
        double yy = origoY + yPos / scaleY;
        
        if(!tool.cyclic){
            xx = std::max(0.0, std::min(xx, (double)tool.data.size()));
        }

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
    terminal.put_function("cyclic", [&](ui::Command c){ switch_cyclic(c); });
    terminal.put_function("lines", [&](ui::Command c){ config_lines(c); });
    terminal.put_function("preset", [&](ui::Command c){ preset(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });
    
    terminal.document("name", "[name] set output name");
    terminal.document("size", "[size] set interval size");
    terminal.document("c", "[arg] [value] set brush config value");
    terminal.document("nil", "sets all data values to zero");
    terminal.document("cyclic", "switch line cyclicness");
    terminal.document("line", "[up/down] [offset] move boundary lines");
    terminal.document("preset", "[type] load a preset layout");
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
    saver.write_double(canvas.upHeight);
    saver.write_double(canvas.downHeight);
    saver.write_bool(cyclic);
    saver.write_int(position);
    saver.write_int(length);

    saver.write_unsigned(data.size());
    for(float i : data) saver.write_float(i);
    
    
    saver.write_float(slider.targetHeight);
    
    canvas.save(saver);
}

void CanvasTool::load(ui::Loader &loader){
    
    output = loader.read_string();
    brush = loader.read_string();
    phase = loader.read_float();
    brushSize = loader.read_float();
    magnitude = loader.read_float();
    frequency = loader.read_unsigned();
    canvas.upHeight = loader.read_double();
    canvas.downHeight = loader.read_double();
    cyclic = loader.read_bool();
    position = loader.read_int();
    length = loader.read_int();

    data.resize(loader.read_unsigned());
    for(float &i : data) i = loader.read_float();
    
    slider.set_target_size(0, loader.read_float());
    
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
            c.source.push_error("type \""+type+"\" not in [phase, mag, size, freq]");
        }
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void CanvasTool::nil_data(ui::Command c){
    newdo();
    for(float &i : data) i = 0.0f;
    update_data();
}

void CanvasTool::switch_cyclic(ui::Command c){
    cyclic ^= 1;
    position = 0;
    update_data();
}

void CanvasTool::config_lines(ui::Command c){
    try {
        std::string dir = c.pop();
        if(dir.empty()){
            c.source.push_error("specify which line you want to move up/down");
            return;
        }
        if(dir[0] == 'u'){
            canvas.upHeight = std::stof(c.pop());
        } else {
            canvas.downHeight = std::stof(c.pop());
        }
        canvas.set_refresh();
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }
}

void CanvasTool::preset(ui::Command c){
    
    auto type = c.pop();

    if(type == "time"){
        
        canvas.upHeight = canvas.downHeight = 1;
        if(!cyclic) switch_cyclic(ui::Command(c.source, {}));
        else canvas.set_refresh();

    } else if(type == "amp"){
        
        canvas.upHeight = 1;
        canvas.downHeight = 0;
        if(cyclic) switch_cyclic(ui::Command(c.source, {}));
        else canvas.set_refresh();
        
    } else if(type == "phase"){

        canvas.upHeight = canvas.downHeight = PI;
        if(cyclic) switch_cyclic(ui::Command(c.source, {}));
        else canvas.set_refresh();

    } else {
        c.source.push_error("type not in [time, amp, phase]");
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

void CanvasTool::draw(double x, double y){

    if(brush == "packet"){
        
        double l = x - brushSize;
        double r = x + brushSize;

        int left = std::ceil(l);
        int right = std::floor(r);

        int size = data.size();

        if(cyclic){
            for(int i=left, j=(left%size+size)%size; i<=right; i++, j=(j+1)%size){
                data[j] += magnitude * y * (0.5f - 0.5f * std::cos((i-l) * PIF / brushSize))
                    * std::cos(2*PIF*(i-x)*frequency / size);
            }
        } else {
            left = std::max(0, left);
            right = std::min((int)data.size()-1, right);
            for(int i=left; i<=right; i++){
                data[i] += magnitude * y * (0.5f - 0.5f * std::cos((i-l) * PIF / brushSize))
                    * std::cos(2*PIF*(i-x)*frequency / size);
            }
        }
    }
    else if(brush == "damp"){

        double l = x - brushSize;
        double r = x + brushSize;

        int left = std::ceil(l);
        int right = std::floor(r);

        int size = data.size();

        if(cyclic){
            for(int i=left, j=(left%size+size)%size; i<=right; i++, j=(j+1)%size){
                data[j] *= std::exp(-magnitude * (0.5 - 0.5 * std::cos((i-l) * PI / brushSize)));
            }
        } else {
            left = std::max(0, left);
            right = std::min((int)data.size()-1, right);
            for(int i=left; i<=right; i++){
                data[i] *= std::exp(-magnitude * (0.5 - 0.5 * std::cos((i-l) * PI / brushSize)));
            }
        }
    }
    else if(brush == "avg"){

        double l = x - brushSize;
        double r = x + brushSize;

        int left = std::ceil(l);
        int right = std::floor(r);

        int size = data.size();

        if(cyclic){
            
            double avg = 0.0, sum = 0.0;
            
            for(int i=left, j=(left%size+size)%size; i<=right; i++, j=(j+1)%size){
                const double c = magnitude * (0.5 - 0.5 * std::cos((i-l) * PI / brushSize));
                avg += data[j] * c;
                sum += c;
            }
            
            avg /= sum;
            
            for(int i=left, j=(left%size+size)%size; i<=right; i++, j=(j+1)%size){
                const double c = magnitude * (0.5 - 0.5 * std::cos((i-l) * PI / brushSize));
                data[j] = (1.0 - c) * data[j] + avg * c;
            }

        } else {
            
            left = std::max(0, left);
            right = std::min((int)data.size()-1, right);
            
            double avg = 0.0, sum = 0.0;
            
            for(int i=left; i<=right; i++){
                const double c = magnitude * (0.5 - 0.5 * std::cos((i-l) * PI / brushSize));
                avg += data[i] * c;
                sum += c;
            }
            
            avg /= sum;
            
            for(int i=left; i<=right; i++){
                const double c = magnitude * (0.5 - 0.5 * std::cos((i-l) * PI / brushSize));
                data[i] = (1.0 - c) * data[i] + avg * c;
            }
        }
    }

    update_data();
}

void CanvasTool::update_data(){

    std::vector<float> show;
    if(cyclic){
        show.resize(length);
        for(int i=0; i<length; i++) show[i] = data[(i+position)%data.size()];
    } else {
        show = data;
    }

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
        for(unsigned i=0; i+32 < undoh.size(); i++) undoh[i].swap(undoh[i+32]);
        undoh.resize(undoh.size() - 32);
    }
}

}

