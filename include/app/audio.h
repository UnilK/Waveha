#pragma once

#include "wave/source.h"
#include "wave/cache.h"
#include "wave/file.h"
#include "ui/terminal.h"
#include "app/session.h"

#include <vector>
#include <map>
#include <set>
#include <mutex>

namespace app {

class App;
class Audio;

class AudioLink : public wave::Source {

public:

    AudioLink(App&);
    virtual ~AudioLink();

    bool open(std::string link);
    
    // for clarity. returns the name of this source i.e. the link.
    std::string source();

    void seek(unsigned sample);
    unsigned tell();
    
    // does not actually have a size. Returns size of source.
    unsigned size();

    unsigned pull(unsigned amount, std::vector<float> &samples);
    std::vector<float> get(unsigned amount, unsigned begin);

    bool pop_update();
    bool is_updated();

private:

    App &app;
    const std::string id;
    unsigned position = 0;

};



class AudioDir : public ui::Directory {

public:

    AudioDir(Audio&);

private:

    Audio &audio;

    void list_sources(ui::Command);
    void link_audio(ui::Command);
    void write_cache(ui::Command);
    void delete_audio(ui::Command);

};



class Audio : public Presistent {

public:

    Audio(App*);
    virtual ~Audio();

    void save(ui::Saver&);
    void load(ui::Loader&);
    void reset();

    bool key_exists(std::string key);

    int add_cache(wave::Audio*);
    int add_file(std::string name, std::string file);
    int remove_audio(std::string name);
    int write_cache(std::string name, std::string file);

    unsigned audio_frameRate(std::string source);
    unsigned audio_channels(std::string source);
    unsigned audio_size(std::string source);
    std::vector<float> get_audio(std::string source, unsigned amount, unsigned position);
    std::vector<float> loop_audio(std::string source, unsigned amount, unsigned position);

    bool pop_update(std::string name, std::string reciever);
    bool is_updated(std::string name, std::string reciever);
    bool set_link(std::string reciever, std::string source);
    bool pop_link(std::string reciever, std::string source);
    
    static std::string generate_reciever_id();

    AudioDir dir;
    friend class AudioDir;

private:

    App &app;

    std::map<std::string, std::set<std::string> > links, updates;

    std::map<std::string, std::string> files;
    std::map<std::string, wave::Audio*> caches;
    std::map<std::string, wave::Source*> sources;

    std::recursive_mutex mutex;

    static std::set<std::string> used_ids;

};

}
