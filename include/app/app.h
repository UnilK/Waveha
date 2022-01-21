#pragma once

#include "ui/window.h"
#include "ui/box.h"
#include "ui/terminal.h"
#include "app/tools.h"
#include "app/audio.h"
#include "app/content.h"
#include "app/creations.h"

namespace app {

class App : public ui::Window {

public:

    App();
    ~App();

    ContentManager layout;
    AudioManager audio;
    CreationManager creation;

    Tools tools;
    ui::Terminal terminal;

    ui::Box terminalBox;
    ui::Slider layoutSlider;

};

}
