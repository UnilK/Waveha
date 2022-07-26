#pragma once

#include "app/content.h"
#include "app/audio.h"
#include "tools/graph.h"
#include "ui/terminal.h"
#include "ui/slider.h"
#include "ui/button.h"
#include "wave/sound.h"
#include "wave/cache.h"

#include <tuple>

namespace app {

class App;

class Labler : public Content {

public:

    Labler(App*);
    ~Labler();
    
    Capture on_event(sf::Event event, int priority);
    
    void save(ui::Saver&);
    void load(ui::Loader&);

    void switch_audio();
    void set_label();
    void clear_label();
    void auto_label();
    void next_data();

    void switch_bit(int i, int j);

    std::string content_type();

    void link_data(ui::Command);
    void info(ui::Command);

private:

    App &app;

    ui::Slider upper, lower, left;
    ui::Terminal terminal;
    ui::Frame grid;
    Graph time, frequency;

    std::vector<std::string> phonetics;
    std::vector<std::tuple<int, int, ui::Button*> > buttons;

    std::string data, labler;

    wave::Audio audio;
    wave::Cache cache;
    wave::Player player;

    bool playing = 0;
    std::vector<char> label;

};

}

