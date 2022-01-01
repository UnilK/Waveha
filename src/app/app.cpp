#include "app/app.h"

#include <iostream>

App::App() :
    Core("res/styles/default.style", 1000),
    window(this)
{
    update_children();
    update_address(id);

    commandHelp = "Root directory.";
    commandDocs = {{"none", "none"}};

}

int32_t App::update_children(){
    commandChildren = {&sessionCommands, &boxCommands, &tabCommands};
    return 0;
}

