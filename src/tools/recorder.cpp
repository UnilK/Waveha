#include "tools/recorder.h"
#include "app/app.h"

#include <assert.h>
#include <string>
#include <iostream>

namespace app {

Recorder::Recorder(App *a) :
    Content(a),
    app(*a),
    timeDisplay(a, "time:", {.look = "basetext", .height = 20, .border = {0, 0, 0, 1}}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    audio("untitled", 1, 44100, {}),
    cache(audio),
    player(cache),
    clock(1)
{

    setup_grid(2, 1);
    fill_width(0, 1);
    fill_height(1, 1);
    
    timeDisplay.text_stick(ui::Text::left);
    timeDisplay.text_offset(1, 0);

    put(0, 0, &timeDisplay);
    put(1, 0, &terminal);

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");

    terminal.put_function("list", [&](ui::Command c){ list_sources(c); });
    terminal.put_function("info", [&](ui::Command c){ source_info(c); });
    terminal.put_function("set", [&](ui::Command c){ set_source(c); });
    terminal.put_function("fuze", [&](ui::Command c){ set_fuze(c); });
    terminal.put_function("rec", [&](ui::Command c){ switch_record(c); });
    terminal.put_function("play", [&](ui::Command c){ switch_playback(c); });
    terminal.put_function("save", [&](ui::Command c){ save_audio(c); });

    terminal.document("list", "list audio sources");
    terminal.document("info", "list info of current source");
    terminal.document("set", "[index] set current audio source");
    terminal.document("fuze", "[time] set delay before recording starts");
    terminal.document("rec", "switch recording on/off");
    terminal.document("play", "switch playback on/off");
    terminal.document("save", "save the audio to caches");

}

void Recorder::save(Saver &save){}

void Recorder::load(Loader &loader){}

std::string Recorder::content_type(){ return "record"; }

void Recorder::on_tick(){
    
    if(recording){
        
        if(!isRecording && clock.get_duration() >= fuze){
            isRecording = 1;
            recorder.start();
        }

        timeDisplay.set_text("time: " + std::to_string(clock.get_duration() - fuze));
    }
    else if(playing){

        timeDisplay.set_text("time: " + std::to_string(clock.get_duration()));

        if(player.getStatus() != wave::Player::Playing) playing = 0;

    }

}

void Recorder::list_sources(ui::Command c){
    
    std::vector<std::string> devices = sf::SoundRecorder::getAvailableDevices();


    for(unsigned i=0; i<devices.size(); i++){
        c.source.push_output(std::to_string(i) + ": " + devices[i]);
    }

}

void Recorder::source_info(ui::Command c){

    c.source.push_output("device: " + recorder.getDevice());
    c.source.push_output("frame rate: " + std::to_string(recorder.getSampleRate()));
    c.source.push_output("channels: " + std::to_string(recorder.getChannelCount()));

}

void Recorder::set_source(ui::Command c){
    
    auto devices = sf::SoundRecorder::getAvailableDevices();

    unsigned src = std::stoi(c.pop());

    if(src > devices.size()){
        c.source.push_error("device index out of bounds.");
    }
    else {
        recorder.setDevice(devices[src]);
    }

}

void Recorder::set_fuze(ui::Command c){
    fuze = std::stof(c.pop());
}

void Recorder::switch_record(ui::Command c){
    
    if(playing) switch_playback(c);

    if(recording){
        
        recording = 0;
        recorder.stop();

        audio.channels = recorder.getChannelCount();
        audio.frameRate = recorder.getSampleRate();
        audio.data = recorder.pull(recorder.max());

        cache.open(audio);

    }
    else {
        
        recording = 1;
        isRecording = 0;
        clock.force_sync_tick();

    }

}

void Recorder::switch_playback(ui::Command c){

    if(recording) switch_record(c);

    if(playing){

        playing = 0;
        player.stop();

    }
    else {
        
        playing = 1;
        cache.seek(0);
        player.play();
        clock.force_sync_tick();

    }

}

void Recorder::save_audio(ui::Command c){

    std::string name = c.pop();

    if(!app.audio.cache(name, audio.channels, audio.frameRate, audio.data)){
        c.source.push_error("invalid cache name");
    }

}

struct initRecorder {
    initRecorder(){ Slot::add_content_type("record", [&](App *a){ return new Recorder(a); } ); }
} iRecorder;

}

