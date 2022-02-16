#pragma once

#include "app/slot.h"
#include "tools/graph.h"
#include "ui/terminal.h"
#include "ui/slider.h"
#include "change/matrix.h"
#include "wave/source.h"
#include "wave/audio.h"

#include <SFML/Graphics/VertexArray.hpp>

namespace app {

class App;

class Meditor : public Content {

public:

    Meditor(App*);
    ~Meditor();
    
    void save(Saver&);
    void load(Loader&);

    void on_tick();

    std::string content_type();

    void update_output();

    void resize_matrix(ui::Command);
    void shuffle_matrix(ui::Command);
    void unit_matrix(ui::Command);
    void rename_output(ui::Command);
    void link_audio(ui::Command);

private:
    
    static const std::string type;
    static int init_class;

    App &app;

    std::string linkId;
    std::string outputName;
    std::string sourceName;

    wave::Source *source = nullptr;

    change::Matrix matrix;

    ui::Terminal terminal;

};

}

