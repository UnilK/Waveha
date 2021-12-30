#include "app/app.h"

#include "app/box.h"
#include "app/tab.h"

#include <iostream>

int32_t App::execute_command(ui::Command &cmd){
    
    if(execute_standard(cmd)){
        return 0;
    } else {
        std::cout << "##" << cmd.command << '\n';
    }

    return 1;
}

FileCommands::FileCommands(){
    id = "file";
    commandHelp = "save & load sessions";
    commandDocs = {
        {"save __name", "save session"},
        {"load _name", "load session"},
        {"new _name", "start new session"},
        {"rename _name", "rename session"}};
}

int32_t FileCommands::execute_command(ui::Command &cmd){
   
    if(execute_standard(cmd)) return 0;
    
    return 0;
}



BoxCommands::BoxCommands(){
    id = "box";
    commandHelp = "manage boxes";
    commandDocs = {
        {"new _name", "create new nocontentbox"},
        {"place _box _tab", "place box to tab"},
        {"detach _box", "detach box from tab"}};
}

int32_t BoxCommands::execute_command(ui::Command &cmd){

    if(execute_standard(cmd)) return 0;

    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;

    App &app = *(App*)commandParent;
        
    if(prefix == "new"){
        
        std::string name;
        cin >> name;

        NoContentBox *pointer = new NoContentBox(&app.window.main, name);

        app.window.main.add_box(pointer);

        return 0;
    } else if(prefix == "place"){

        std::string box, tab;
        cin >> box >> tab;

        Box *boxp = app.window.main.find_box(box);
        Tab *tabp = app.window.main.find_tab(tab);

        if(boxp != nullptr && tabp != nullptr){
            boxp->place_to_tab(tabp);
        }
        
        return 0;
    } else if(prefix == "detach"){
        
        std::string box;
        cin >> box;

        Box *boxp = app.window.main.find_box(box);

        if(boxp != nullptr){
            boxp->detach_from_tab();
        }
        return 0;
    } else {
        std::cout << "command not recognized. try \"help\"\n";
        return 1;
    }

}



TabCommands::TabCommands(){
    id = "tab";
    commandHelp = "manage tabs";
    commandDocs = {
        {"new _name", "create new tab"}};
}

int32_t TabCommands::execute_command(ui::Command &cmd){

    if(execute_standard(cmd)) return 0;

    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;
        
    App &app = *(App*)commandParent;
    
    if(prefix == "new"){
        
        std::string name;
        cin >> name;

        Tab *pointer = new Tab(&app.window.main, name);

        app.window.main.add_tab(pointer);

        return 0;
    } else {
        std::cout << "command not recognized. try \"help\"\n";
        return 1;
    }
}

