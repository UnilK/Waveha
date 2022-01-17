#pragma once

#include "ui/slider.h"

namespace app {

class Tools : public ui::Slider {
    
public:

    Tools(ui::Window *master_);

private:

    ui::Slider dummy;

};

}
