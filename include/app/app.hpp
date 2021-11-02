#pragma once

class App;

#include "ui/core.h"
#include "ui/window.h"

#include "app/style.hpp"
#include "app/mainFrame.hpp"

class App : public ui::Core {

public:

    MainWindow window;

    App() : Core(1, 1), window(this) {
        style = defaultStyle;
    }

    int32_t execute_command(std::string command){
        return 0;
    }

};

