#pragma once

#include "app/content.h"
#include "app/audio.h"
#include "tools/graph.h"
#include "ui/terminal.h"
#include "ui/slider.h"
#include "change/matrix.h"

#include <SFML/Graphics/VertexArray.hpp>

namespace app {

class App;

class Meditor : public Content {

public:

    Meditor(App*);
    ~Meditor();
    
    void save(ui::Saver&);
    void load(ui::Loader&);

    Capture on_event(sf::Event event, int priority);
    
    void on_tick();

    std::string content_type();

    void update_output();

    void resize_matrix(ui::Command);
    void shuffle_phase(ui::Command);
    void shuffle_magnitude(ui::Command);
    void set_phase(ui::Command);
    void set_magnitude(ui::Command);
    void unit_matrix(ui::Command);
    void rename_output(ui::Command);
    void link_audio(ui::Command);
    void set_stack(ui::Command);
    void set_mode(ui::Command);
    void set_auto(ui::Command);
    void info(ui::Command);

    void set_slant(ui::Command);
    void set_pitch(ui::Command);
    void clink_audio(ui::Command);
    void set_cpitch(ui::Command);
    void remember(ui::Command);

private:

    App &app;

    AudioLink link, lphase, gphase, amplitude;

    change::Matrix matrix;

    std::string outputName = "untitled";

    bool autoRefresh = 0;
    int mode = 0;
    std::string stack = "";

    ui::Terminal terminal;

    // temporary experiments

    float slantIn = 1, slantOut = 1, pitch = 1, cpitch = 120.0f;
    std::vector<float> rmags;

};

}

