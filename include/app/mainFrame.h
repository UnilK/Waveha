#pragma once

#include "ui/frame.h"

class MainFrame : ui::Frame{

protected:

public:

    MainFrame();
    MainFrame(ui::Core *core_, ui::Window *master_, std::map<std::string, std::string> values = {});

};

