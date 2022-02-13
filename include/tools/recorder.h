#pragma once

#include "app/slot.h"
#include "ui/button.h"
#include "ui/clock.h"
#include "ui/terminal.h"
#include "wave/cache.h"
#include "wave/mic.h"
#include "wave/sound.h"

namespace app {

class Recorder : public Content {

public:

    Recorder(App*);

    void save(Saver&);
    void load(Loader&);

    std::string content_type();
    
    void on_tick();

    void list_sources(ui::Command);
    void source_info(ui::Command);
    void set_source(ui::Command);
    void set_fuze(ui::Command);
    void switch_record(ui::Command);
    void switch_playback(ui::Command);
    void save_audio(ui::Command);

private:
    
    static const std::string type;
    static int init_class;

    App &app;

    ui::Text timeDisplay;
    ui::Terminal terminal;

    wave::Mic recorder;
    wave::Audio audio;
    wave::Cache cache;
    wave::Player player;

    ui::Clock clock;

    bool recording = 0, playing = 0;

    bool isRecording = 0;
    double fuze = 0;

};

}

