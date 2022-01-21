#pragma once

#include "ui/slider.h"

namespace app {

class App;

class Tools : public ui::Slider {
    
public:

    Tools(App *master_);

private:

    App &app;

};

}
