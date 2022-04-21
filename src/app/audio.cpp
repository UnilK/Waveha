#include "app/audio.h"
#include "app/app.h"
#include "wstream/constants.h"

#include <string.h>
#include <algorithm>

namespace app {

// audio link /////////////////////////////////////////////////////////////////

AudioLink::AudioLink(App &a) :
    app(a), id(Audio::generate_reciever_id())
{
    frameRate = 44100;
    channels = 1;
}

AudioLink::~AudioLink(){
    app.audio.pop_link(id, source());
}

bool AudioLink::open(std::string link){
    app.audio.pop_link(id, source());
    position = 0;
    name = link;
    channels = app.audio.audio_channels(link);
    frameRate = app.audio.audio_frameRate(link);
    app.audio.set_link(id, source());
    return app.audio.key_exists(source());
}

std::string AudioLink::source(){
    return name;
}

void AudioLink::seek(unsigned sample){
    if(size() > 0) position = std::min(sample, size()-1);
}

unsigned AudioLink::tell(){
   return position;
}

unsigned AudioLink::size(){
    return app.audio.audio_size(source());
}

unsigned AudioLink::pull(unsigned amount, std::vector<float> &samples){

    good = 1;

    unsigned actual = std::min(amount, (unsigned)size() - position);

    if(amount != actual) good = 0;

    samples = app.audio.get_audio(source(), amount, position);

    position += actual;

    return actual;
}

std::vector<float> AudioLink::get(unsigned amount, unsigned begin){
    return app.audio.get_audio(source(), amount, begin);
}
    
bool AudioLink::pop_update(){
    return app.audio.pop_update(id, source());
}

bool AudioLink::is_updated(){
    return app.audio.is_updated(id, source());
}

// audio //////////////////////////////////////////////////////////////////////

Audio::Audio(App *a) :
    dir(*this),
    app(*a)
{}

Audio::~Audio(){}

void Audio::save(ui::Saver &saver){
    
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

void Audio::load(ui::Loader &loader){

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

    links.clear();
    updates.clear();
}

bool Audio::key_exists(std::string key){
    return sources.count(key);
}

int Audio::add_cache(wave::Audio *audio){

    set_lock(audio->name, 1);
    
    int ret = key_exists(audio->name);
    if(ret){
        set_lock(audio->name, 0);
        remove_audio(audio->name);
        set_lock(audio->name, 1);
    }

    caches[audio->name] = audio;
    sources[audio->name] = new wave::Cache(audio);
    if(!ret) locks[audio->name] = new std::mutex();
    for(auto i : links[audio->name]) updates[audio->name].insert(i);

    set_lock(audio->name, 0);

    return ret;
}

int Audio::add_file(std::string name, std::string file){
    
    set_lock(name, 1);
    
    int ret = key_exists(name);
    if(ret){
        set_lock(name, 0);
        remove_audio(name);
        set_lock(name, 1);
    }

    iwstream I;

    if(!I.open(file)){
        set_lock(name, 0);
        return 2;
    }

    files[name] = file;
    sources[name] = new wave::File(file);
    if(!ret) locks[name] = new std::mutex();
    for(auto i : links[name]) updates[name].insert(i);

    set_lock(name, 0);
    
    return ret;
}

int Audio::remove_audio(std::string name){
    
    set_lock(name, 1);
    if(!key_exists(name)){
        set_lock(name, 0);
        return 1;
    }

    if(files.count(name)){
        files.erase(name);
    }
    else if(caches.count(name)){
        delete caches[name];
        caches.erase(name);
    }

    delete sources[name];
    sources.erase(name);
    
    set_lock(name, 0);

    return 0;
}

int Audio::write_cache(std::string name, std::string file){
    
    set_lock(name, 1);
    if(!caches.count(name)){
        set_lock(name, 0);
        return 1;
    }

    owstream O("audio/"+file+".wav", PCM, caches[name]->channels, 16, caches[name]->frameRate);

    O.write_file(caches[name]->data);

    set_lock(name, 0);
    
    return 0;
}

unsigned Audio::audio_frameRate(std::string source){
    if(sources.count(source) == 0) return 44100;
    return sources[source]->frameRate;
}

unsigned Audio::audio_channels(std::string source){
    if(sources.count(source) == 0) return 1;
    return sources[source]->channels;
}

unsigned Audio::audio_size(std::string source){
    if(sources.count(source) == 0) return 0;
    return sources[source]->size();
}

std::vector<float> Audio::get_audio(std::string source, unsigned amount, unsigned position){
    
    if(sources.count(source) == 0){
        return std::vector<float>(amount, 0.0f);
    }
    set_lock(source, 1);
    std::vector<float> ret = sources[source]->get(amount, position);
    set_lock(source, 0);
    return ret;
}

std::vector<float> Audio::loop_audio(std::string source, unsigned amount, unsigned position){
    
    if(sources.count(source) == 0){
        return std::vector<float>(amount, 0.0f);
    }
    set_lock(source, 1);
    std::vector<float> ret = sources[source]->get_loop(amount, position);
    set_lock(source, 0);
    return ret;
}

bool Audio::pop_update(std::string reciever, std::string source){
    if(!sources.count(source)) return 0;
    bool ret = updates[source].count(reciever);
    updates[source].erase(reciever);
    return ret;
}

bool Audio::is_updated(std::string reciever, std::string source){
    if(!sources.count(source)) return 0;
    return updates[source].count(reciever);
}

bool Audio::set_link(std::string reciever, std::string source){
    if(sources.count(source)){
        links[source].insert(reciever);
        return 1;
    }
    return 0;
}

bool Audio::pop_link(std::string reciever, std::string source){
    if(sources.count(source)){
        links[source].erase(reciever);
        return 1;
    }
    return 0;
}

void Audio::set_lock(std::string source, bool state){
    if(key_exists(source)){
        if(state){
            lockCount[source]++;
            locks[source]->lock();
        }
        else {
            locks[source]->unlock();
            if(lockCount[source] > 0) lockCount[source]--;
            if(lockCount[source] == 0 && !key_exists(source)){
                lockCount.erase(source);
                delete locks[source];
                locks.erase(source);
            }
        }
    }
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
    document("save", "[name] [file] save a cache to a .wav file");
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
    
    auto name = c.pop(), file = c.pop();
    
    if(name.empty() || file.empty()){
        c.source.push_error("give a name and a file");
    }
    else {
        int ret = audio.write_cache(name, file);
        if(ret) c.source.push_error("cache not found");
    }
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
