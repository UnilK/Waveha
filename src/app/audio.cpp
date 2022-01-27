#include "app/audio.h"
#include "app/app.h"

namespace app {

// audio //////////////////////////////////////////////////////////////////////

Audio::Audio(App *a) :
    dir(*this),
    app(*a)
{}

bool Audio::key_exists(std::string key){
    return files.count(key) != 0 || caches.count(key) != 0;
}

bool Audio::cache(
        std::string name,
        uint32_t channels,
        uint32_t frameRate,
        const std::vector<float> &data)
{
    if(key_exists(name)) return 0;

    caches[name] = new wave::Audio(name, channels, frameRate, data);

    return 1;
}

bool Audio::link(std::string name, std::string file){
    
    if(key_exists(name)) return 0;

    iwstream I;

    if(!I.open(file)) return 0;

    files[name] = file;

    return 1;
}

bool Audio::remove_audio(std::string name){
    
    if(!key_exists(name)) return 0;
    
    if(files.count(name)){
        files.erase(name);
    }
    else if(caches.count(name)){
        delete caches[name];
        caches.erase(name);
    }

    return 1;
}

wave::Source *Audio::get_source(std::string name){
    
    if(files.count(name)) return new wave::File(files[name]);
    if(caches.count(name)) return new wave::Cache(*caches[name]);

    return nullptr;
}

// directory //////////////////////////////////////////////////////////////////

AudioDir::AudioDir(Audio &a) : audio(a) {

    put_function("list", [&](ui::Command c){ list_sources(c); });
    put_function("link", [&](ui::Command c){ link_audio(c); });
    put_function("save", [&](ui::Command c){ write_cache(c); });
    put_function("del", [&](ui::Command c){ delete_audio(c); });

}

void AudioDir::list_sources(ui::Command c){
    
    c.source.push_output("files:");
    
    if(audio.files.empty()){
        c.source.push_output("(empty)");
    }
    else {
        for(auto i : audio.files) c.source.push_output(i.first);
    }

    c.source.push_output("\ncahces:");

    if(audio.caches.empty()){
        c.source.push_output("(empty)");
    }
    else {
        for(auto i : audio.caches) c.source.push_output(i.first);
    }
    
}

void AudioDir::link_audio(ui::Command c){
    
    std::string file = c.pop();
    std::string name = file;
    if(!c.empty()) name = c.pop();

    if(audio.key_exists(name)){
        c.source.push_error("source with name \"" + name + "\" already exsists");
        return;
    }

    if(!audio.link(name, "audio/" + file + ".wav")){
        c.source.push_error("error opening file: audio/" + file + ".wav");
    }

}

void AudioDir::write_cache(ui::Command c){
    c.source.push_output("not implemented yet.");
}

void AudioDir::delete_audio(ui::Command c){
    
    std::string name = c.pop();

    if(!audio.remove_audio(name)){
        c.source.push_error("source with name \"" + name + "\" not found");
        return;
    }

}

}
