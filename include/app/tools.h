#pragma once

#include "ui/slider.h"
#include "ui/box.h"

namespace app {

class Tools : public ui::Slider {
    
public:

    Tools(ui::Window *master_);

private:

    ui::Box dummy0, dummy1, dummy2;

};

}
