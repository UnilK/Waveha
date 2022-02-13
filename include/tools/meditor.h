#pragma once

#include "app/slot.h"
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
    
    void save(Saver&);
    void load(Loader&);

    std::string content_type();

    void resize_matrix(ui::Command);
    void shuffle_matrix(ui::Command);
    void unit_matrix(ui::Command);

    void update_data();
    void set_begin(unsigned);
    void set_end(unsigned);

private:
    
    static const std::string type;
    static int init_class;

    change::Matrix matrix;

};

}

