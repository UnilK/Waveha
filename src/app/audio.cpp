#include "app/audio.h"
#include "app/app.h"

#include <string.h>

namespace app {

// audio //////////////////////////////////////////////////////////////////////

Audio::Audio(App *a) :
    dir(*this),
    app(*a)
{}

Audio::~Audio(){
    app.audioAlive = 0;
}

void Audio::save(Saver &saver){
    
    saver.write_unsigned(files.size());

    for(auto i : files){
        saver.write_string(i.first);
        saver.write_string(i.second);
    }

    saver.write_unsigned(caches.size());

    for(auto i : caches){
        saver.write_string(i.first);
        saver.write_unsigned(i.second->channels);
        saver.write_unsigned(i.second->frameRate);
        saver.write_block(i.second->data.size() * sizeof(float), i.second->data.data());
    }
}

void Audio::load(Loader &loader){

    reset();

    unsigned famount = loader.read_unsigned();

    for(unsigned i=0; i<famount; i++){
        auto name = loader.read_string();
        auto file = loader.read_string();
        add_file(name, file);
    }

    unsigned camount = loader.read_unsigned();

    for(unsigned i=0; i<camount; i++){

        wave::Audio *audio = new wave::Audio();
        audio->name = loader.read_string();
        audio->channels = loader.read_unsigned();
        audio->frameRate = loader.read_unsigned();
        
        auto data = loader.read_block();

        audio->data.resize(data.size() / sizeof(float));

        memcpy((char*)audio->data.data(), data.data(), data.size());
    
        add_cache(audio);

    }
}

void Audio::reset(){
    
    std::vector<std::string> keys;
    for(auto i : files) keys.push_back(i.first);
    for(auto i : caches) keys.push_back(i.first);

    for(auto i : keys) remove_audio(i);
}

bool Audio::pop_update(std::string name, std::string reciever){
    bool ret = is_updated(name, reciever);
    updates[name].erase(reciever);
    return ret;
}

bool Audio::is_updated(std::string name, std::string reciever){
    return updates[name].count(reciever);
}


bool Audio::key_exists(std::string key){
    return files.count(key) != 0 || caches.count(key) != 0;
}

int Audio::add_cache(wave::Audio *audio){

    int ret = key_exists(audio->name);

    if(ret) remove_audio(audio->name);

    caches[audio->name] = audio;
    for(auto i : links[audio->name]) updates[audio->name].insert(i);

    return ret;
}

int Audio::add_file(std::string name, std::string file){
    
    bool ret = key_exists(name);

    if(ret) remove_audio(name);

    iwstream I;

    if(!I.open(file)) return 2;

    files[name] = file;
    for(auto i : links[name]) updates[name].insert(i);

    return ret;
}

int Audio::remove_audio(std::string name){
    
    if(!key_exists(name)) return 1;
    if(!links[name].empty()) return 2;
    
    if(files.count(name)){
        files.erase(name);
    }
    else if(caches.count(name)){
        delete caches[name];
        caches.erase(name);
    }

    return 1;
}


wave::Source *Audio::get_source(std::string name, std::string reciever){
    
    if(files.count(name)){
        links[name].insert(reciever);
        return new wave::File(files[name]);
    }

    if(caches.count(name)){
        links[name].insert(reciever);
        return new wave::Cache(caches[name]);
    }

    return nullptr;
}

void Audio::detach_source(std::string name, std::string reciever){
    links[name].erase(reciever);
}

std::string Audio::generate_reciever_id(){
    
    std::string id;

    do {
        id.clear();
        id.push_back('#');
        for(int i=0; i<8; i++) id.push_back('0' + rand()%10);
    } while(used_ids.count(id));

    used_ids.insert(id);

    return id;
}

std::set<std::string> Audio::used_ids;

// directory //////////////////////////////////////////////////////////////////

AudioDir::AudioDir(Audio &a) : audio(a) {

    put_function("list", [&](ui::Command c){ list_sources(c); });
    put_function("link", [&](ui::Command c){ link_audio(c); });
    put_function("save", [&](ui::Command c){ write_cache(c); });
    put_function("del", [&](ui::Command c){ delete_audio(c); });

    document("list", "list currently stored caches and linked files");
    document("link", "[file] {name} link a .wav file");
    document("save", "[name] save a cache to a .wav file");
    document("del", "[name] delete a cache or unlink a file");
}

void AudioDir::list_sources(ui::Command c){
    
    std::string message = "";

    message += "files:\n";
    
    if(audio.files.empty()) message += "(empty)\n";
    else for(auto i : audio.files) message += i.first + "\n";

    message += "\ncaches:\n";

    if(audio.caches.empty()) message += "(empty)\n";
    else for(auto i : audio.caches) message += i.first + "\n";

    message.pop_back();

    c.source.push_output(message);
}

void AudioDir::link_audio(ui::Command c){
    
    std::string file = c.pop();
    std::string name = file;
    if(!c.empty()) name = c.pop();

    if(audio.add_file(name, "audio/" + file + ".wav") == 2){
        c.source.push_error("error opening file: audio/" + file + ".wav");
    }

}

void AudioDir::write_cache(ui::Command c){
    c.source.push_output("not implemented yet.");
}

void AudioDir::delete_audio(ui::Command c){
    
    std::string name = c.pop();

    int ret = audio.remove_audio(name);

    if(ret == 1){
        c.source.push_error("source with name \"" + name + "\" not found");
    }
    else if(ret == 2){
        c.source.push_error("source with name \"" + name + "\" is linked."
                " Unlink the source to remove it.");
    }

}

}
