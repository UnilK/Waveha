#pragma once

class App;

#include "ui/core.h"
#include "ui/window.h"

#include "app/mainFrame.h"

#include <iostream>
#include <vector>

class SessionCommands : public ui::Commandable {

public:
    SessionCommands();
    int32_t execute_command(ui::Command &cmd);

};



class BoxCommands : public ui::Commandable {

public:
    BoxCommands();
    int32_t execute_command(ui::Command &cmd);

};



class TabCommands : public ui::Commandable {

public:
    TabCommands();
    int32_t execute_command(ui::Command &cmd);

};



class App : public ui::Core {

public:

    App();

    int32_t execute_command(ui::Command &cmd);

    int32_t update_children();

    friend class FileCommands;

    MainWindow window;

    SessionCommands sessionCommands;
    BoxCommands boxCommands;
    TabCommands tabCommands;

    std::string sessionName = "untitled";
    std::vector<ui::Command> session;

};

