#include "app/app.h"

#include <iostream>

App::App() :
    Core("res/styles/default.style", 1000),
    window(this)
{
    update_children();
    update_address(id);

    commandHelp = "Root directory.";
    commandDocs = {{"yeetus", "geetus"}};

}

int32_t App::execute_command(ui::Command &cmd){
    
    if(execute_standard(cmd)){
        return 0;
    } else {
        std::cout << "##" << cmd.command << '\n';
    }

    return 1;
}

int32_t App::update_children(){
    commandChildren = {&fileCommands};
    return 0;
}

