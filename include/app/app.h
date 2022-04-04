#pragma once

#include "ui/window.h"
#include "ui/box.h"
#include "ui/terminal.h"
#include "app/tools.h"

namespace app {

class Audio;
class Session;
class Creations;
class Layout;

class App : public ui::Window {

public:

    App();
    ~App();

    Audio &audio;
    Session &session;
    Creations &creations;
    Layout &layout;
    
    Tools tools;
    ui::Terminal terminal;
    ui::Box terminalBox;

};

}
