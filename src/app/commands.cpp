#include "app/app.h"

#include <iostream>

int32_t App::execute_command(ui::Command &cmd){
    
    if(execute_standard(cmd)){
        return 0;
    } else {
        std::cout << "##" << cmd.command << '\n';
    }

    return 1;
}

App::FileCommands::FileCommands(){
    id = "file";
    commandHelp = "wololo no help for you lol.";
    commandDocs = {{"wa", "yee"}};
}

int32_t App::FileCommands::execute_command(ui::Command &cmd){
   
    if(!execute_standard(cmd)){
        std::cout << cmd.command << '\n';
    }
    
    return 0;
}

