#pragma once

class App;

#include "ui/core.h"
#include "ui/window.h"

#include "app/mainFrame.h"

#include <iostream>

class App : public ui::Core {

public:

    App();

    int32_t execute_command(ui::Command &cmd);

    int32_t update_children();

protected:

    MainWindow window;

    class FileCommands : public ui::Commandable {
    public:
        FileCommands();
        int32_t execute_command(ui::Command &cmd);
    };

    FileCommands fileCommands;

};

