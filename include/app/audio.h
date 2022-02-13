#pragma once

#include "wave/source.h"
#include "wave/cache.h"
#include "wave/file.h"
#include "wave/loop.h"
#include "ui/terminal.h"
#include "app/session.h"

#include <vector>
#include <map>
#include <set>

namespace app {

class App;
class Audio;

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
    
    void save(Saver&);
    void load(Loader&);
    void reset();

    bool pop_update(std::string key);
    bool is_updated(std::string key);

    bool key_exists(std::string key);

    int add_cache(wave::Audio*);
    int add_file(std::string name, std::string file);

    bool remove_audio(std::string name);

    wave::Source *get_source(std::string name);

    AudioDir dir;

    friend class AudioDir;

private:

    App &app;

    std::set<std::string> updates;

    std::map<std::string, std::string> files;
    std::map<std::string, wave::Audio*> caches;

};

}
