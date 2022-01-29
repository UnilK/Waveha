#pragma once

#include "ui/window.h"
#include "ui/terminal.h"
#include "app/tools.h"
#include "app/audio.h"
#include "app/layout.h"
#include "app/creations.h"
#include "app/session.h"

namespace app {

class App : public ui::Window {

public:

    App();
    ~App();

    Layout layout;
    Audio audio;
    Session session;

    Tools tools;
    ui::Terminal terminal;

    ui::Box terminalBox;

};

}
