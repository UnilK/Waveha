#pragma once

class App;

#include "ui/core.h"
#include "ui/window.h"

#include "app/mainFrame.h"

#include <iostream>

class App : public ui::Core {

protected:

    MainWindow window;

public:

    App();

    int32_t execute_command(std::string command);

};

