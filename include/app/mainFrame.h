#pragma once

#include "ui/frame.h"

class MainFrame : public ui::SolidFrame {

public:

    MainFrame(ui::Window *master_, std::map<std::string, std::string> values = {});

};

class MainWindow : public ui::Window{

    MainFrame main;

public:
    
    MainWindow(ui::Core *core_);
    
    int32_t on_close();

};

