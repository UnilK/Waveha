#include "tools/labler.h"
#include "app/app.h"
#include "app/creations.h"
#include "ml/waves.h"
#include "math/ft.h"

namespace app {

Labler::Labler(App *a) :
    Content(a),
    app(*a),
    upper(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 200, .spanx = 2}),
    lower(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 200, .spanx = 2}),
    left(a, ui::Side::right, ui::Side::left, {.look = "basebox", .width = 400}),
    terminal(a, {.look = "baseterminal"}),
    grid(a, {.look = "basebox"}),
    time(a, {.look = "agraph"}),
    frequency(a, {.look = "agraph"}),
    audio("noname", 1, 44100, {0, 0, 0, 0}),
    cache(&audio),
    player(&cache),
    label(8)
{
    player.set_loop(1);

    setup_grid(3, 2);
    fill_width({0, 1});
    fill_height({1, 0, 0});

    left.stack.set_border(0, 0, 0, 0);
    upper.stack.set_border(0, 0, 0, 0);
    lower.stack.set_border(0, 0, 0, 0);

    put(0, 0, &left);
    put(1, 0, &upper);
    put(2, 0, &lower);
    
    left.set_scrollable(0);
    lower.set_scrollable(0);

    left.stack.create(1);
    left.stack.put(&time, 0);
    left.stack.update();
    left.stack.fill_width({1, 0});
    put(0, 1, &frequency);

    upper.stack.create(1);
    upper.stack.put(&grid, 0);
    upper.stack.update();
    upper.stack.fill_height({1, 0});
    
    lower.stack.create(1);
    lower.stack.put(&terminal, 0);
    lower.stack.update();
    lower.stack.fill_height({1, 0});
    
    grid.setup_grid(4, 16);
    
    for(char &i : label) i = 0;
    phonetics = {
        "a","e","o","u","w","y","ae","oe","","","","","","","","",
        "i","j","g","v","","","","","","","","","","","","",
        "m","n","ng","th","","","","","","","","","","","","",
        "l","r","","","","","","","","","","","","","",""};
    buttons.resize(64);

    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            
            buttons[i*8+j] = {i, j, nullptr};
            auto &[x, y, btn] = buttons[8*i+j];
            
            btn = new ui::Button(a, [&](){ switch_bit(x, y); }, phonetics[i*8+j],
                    {.look = "basebutton", .width = 40, .height = 40,
                    .pad = {2, 2, 2, 2}, .border = {1, 1, 1, 1}});

            btn->text_stick(ui::Button::middle);

            grid.put(i/2, j+i%2*8, btn);
        }
    }

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("link", [&](ui::Command c){ link_data(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });
    
    terminal.document("*hotkeys",
            "A to switch audio playback\n"
            "S clear all bits\n"
            "D set the label\n"
            "F switch auto labeling\n"
            "R discard the data");
    terminal.document("link", "link a dataset to be labeled");
    terminal.document("info", "list the configuration");
}

Labler::~Labler(){
    for(auto [x, y, btn] : buttons) delete btn;
}

ui::Frame::Capture Labler::on_event(sf::Event event, int priority){

    if(event.type == sf::Event::KeyPressed){
        if(event.key.code == sf::Keyboard::A){
            switch_audio();
            return Capture::capture;
        }
        if(event.key.code == sf::Keyboard::S){
            clear_label();
            return Capture::capture;
        }
        if(event.key.code == sf::Keyboard::D){
            set_label();
            return Capture::capture;
        }
        if(event.key.code == sf::Keyboard::F){
            auto_label();
            return Capture::capture;
        }
        if(event.key.code == sf::Keyboard::R){
            discard_data();
            return Capture::capture;
        }
    }
    
    return Capture::pass;
}

void Labler::save(ui::Saver &saver){

    saver.write_string(data);

    saver.write_float(left.targetWidth);
    saver.write_float(upper.targetHeight);
    saver.write_float(lower.targetHeight);

    time.save(saver);
    frequency.save(saver); 
}

void Labler::load(ui::Loader &loader){

    data = loader.read_string();

    left.set_target_size(loader.read_float(), 0);
    upper.set_target_size(0, loader.read_float());
    lower.set_target_size(0, loader.read_float());

    time.load(loader);
    frequency.load(loader);

    next_data();
}

void Labler::switch_audio(){

    if(playing){
        playing = 0;
        player.stop();
    }
    else {
        playing = 1;
        cache.seek(0);
        player.play();
    }
}

void Labler::set_label(){
    
    ml::WaveData *source = (ml::WaveData*)app.creations.get_mldata(data);
    if(source == nullptr) return;
    
    int count = 0;
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(label[i] >> j & 1) count++;
        }
    }

    if(count == 0) return;

    source->label_next(label);
    
    terminal.push_output("progress: " + std::to_string(source->labeled_size())
            + " / " + std::to_string(source->size()));

    next_data();
}

void Labler::clear_label(){
    for(int i=0; i<8; i++){
        for(int j=0; j<8; j++){
            if(label[i] >> j & 1) switch_bit(i, j);
        }
    }
}

void Labler::auto_label(){
    
    // currently acts as a refresh...

    next_data();
}

void Labler::next_data(){

    ml::WaveData *source = (ml::WaveData*)app.creations.get_mldata(data);
    if(source == nullptr) return;
    
    auto [input, label] = source->next_unlabeled();

    if(input.size() != 129){
        input.resize(128, 0.0314f);
        input.push_back(100);
    }

    float pitch = input.back();
    input.pop_back();
    
    std::vector<std::complex<float> > freq(input.size()/2);
    for(unsigned i=0; i<freq.size(); i++) freq[i] = {input[2*i], input[2*i+1]};
    
    auto waves1 = math::ift(freq, (unsigned)(audio.frameRate/pitch));
    auto waves2 = math::ift(freq, 1<<9);
    auto waves3 = waves2;
    for(float i : waves2) waves3.push_back(i);

    time.set_data(waves3);
    frequency.set_data(freq);
    time.set_reconfig();
    frequency.set_reconfig();

    bool wasPlaying = playing;
    if(playing) switch_audio();

    float norm = 0.2f * std::sqrt(120.0f / pitch);

    for(float &i : waves1) i *= norm;

    audio.data = waves1;
    cache.open(&audio);
    player.open(&cache);

    if(wasPlaying) switch_audio();

    terminal.push_output("pitch: " + std::to_string(pitch));

    clear_label();
}

void Labler::discard_data(){

    ml::WaveData *source = (ml::WaveData*)app.creations.get_mldata(data);
    if(source == nullptr) return;
    
    source->discard_next();
    
    terminal.push_output("progress: " + std::to_string(source->labeled_size())
            + " / " + std::to_string(source->size()));

    next_data();
}

void Labler::switch_bit(int i, int j){
    label[i] ^= 1<<j;
    auto &[x, y, btn] = buttons[i*8+j];
    if(label[i] >> j & 1) btn->set_look("inversebutton");
    else btn->set_look("basebutton");
}

namespace Factory { extern std::string labler; }
std::string Labler::content_type(){ return Factory::labler; }

void Labler::link_data(ui::Command c){

    std::string name = c.pop();

    if(name.empty()){
        c.source.push_error("dataset name cannot be empty");
        return;
    }

    if(app.creations.data_type(name) != "wave"){
        c.source.push_error("dataset not found / has wrong type");
        return;
    }

    data = name;
    clear_label();
    next_data();
}

void Labler::info(ui::Command c){

    std::string message;
    message += "dataset: " + data + "\n";
    
    c.source.push_output(message);
}

}

