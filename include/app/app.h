#pragma once

#include "ui/window.h"
#include "ui/frame.h"
#include "app/tools.h"

namespace app {

class App : public ui::Window {

public:

    App();
    ~App();

private:

    Tools tools;
    ui::Frame temp;

};

}
