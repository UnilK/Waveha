#include "app/app.h"

#include "app/box.h"
#include "app/tab.h"

#include <iostream>
#include <fstream>

int32_t App::update_children(){
    
    commandChildren = {
        &sessionCommands,
        &boxCommands,
        &tabCommands,
        &audioCommands,
        &analysisCommands,
        &blingCommands
    };
    
    return 0;
}

int32_t App::execute_command(ui::Command &cmd){
    
    if(execute_standard(cmd)){
        return 0;
    } else {
        std::cout << "unrecognized command: " << cmd.command << '\n';
    }

    return 1;
}

SessionCommands::SessionCommands(){
    id = "session";
    commandHelp = "save & load sessions";
    commandDocs = {
        {"save __name", "save session"},
        {"load _name", "load session"},
        {"new _name", "start new session"},
        {"rename _name", "rename session"}};
}

int32_t SessionCommands::execute_command(ui::Command &cmd){
   
    if(execute_standard(cmd)) return 0;
    
    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;
    
    App &app = *(App*)commandParent;
   
    if(prefix == "save"){
   
        std::string name;

        cin >> name;

        if(name != "") app.sessionName = name;

        std::ofstream file("sessions/"+app.sessionName+".was");

        for(auto mem : app.session){
            file << mem.address << ' ' << mem.command << '\n';
        }

        return 0;

    } else if(prefix == "load"){

        std::string name;

        cin >> name;

        std::ifstream file("sessions/"+name+".was");
        if(!file.good()) return 1;

        app.sessionName = name;
        app.session.clear();

        std::string mem;

        while(std::getline(file, mem)){
            std::stringstream mems(mem);
            ui::Command command = app.create_command(mems);
            app.deliver_address(command);
            app.session.push_back(command);
        }

        return 0;

    } else if(prefix == "new"){

        std::ifstream file;
        std::string name;

        cin >> name;
        
        if(name == "") return 1;

        app.sessionName = name;
        app.session.clear();

        return 0;

    } else if(prefix == "rename"){

        std::ifstream file;
        std::string name;

        cin >> name;

        if(name == "") return 1;

        app.sessionName = name;

        return 0;

    } else {
        
        std::cout << "session command not recognized.\n";
    
    }

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

        app.session.push_back(cmd);

        return 0;
    } else if(prefix == "place"){

        std::string box, tab;
        cin >> box >> tab;

        Box *boxp = app.window.main.find_box(box);
        Tab *tabp = app.window.main.find_tab(tab);

        if(boxp != nullptr && tabp != nullptr){
            boxp->place_to_tab(tabp);
        }
        
        app.session.push_back(cmd);
        
        return 0;
    } else if(prefix == "detach"){
        
        std::string box;
        cin >> box;

        Box *boxp = app.window.main.find_box(box);

        if(boxp != nullptr){
            boxp->detach_from_tab();
        }
        
        app.session.push_back(cmd);
        
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
        
        app.session.push_back(cmd);

        return 0;
    } else {
        std::cout << "command not recognized. try \"help\"\n";
        return 1;
    }
}


AudioCommands::AudioCommands(){
    id = "audio";
    commandHelp = "manage audio";
    commandDocs = {
        {"link _name", "link audio"}};
}

int32_t AudioCommands::execute_command(ui::Command &cmd){
    return 1;
}



AnalysisCommands::AnalysisCommands(){
    id = "analysis";
    commandHelp = "analyze audio";
    commandDocs = {
        {"analyze _name", "analyze linked or cached audio"}};
}

int32_t AnalysisCommands::execute_command(ui::Command &cmd){
    return 1;
}

BlingCommands::BlingCommands(){
    id = "bling";
    commandHelp = "manage styles";
    commandDocs = {
        {"_name", "load syle file"}};
}

int32_t BlingCommands::execute_command(ui::Command &cmd){
    
    if(execute_standard(cmd)) return 0;

    std::stringstream cin(cmd.command);
    std::string fileName;

    cin >> fileName;
    
    std::ifstream I("res/styles/"+fileName+".style");

    if(I.good()){
        App &app = *(App*)commandParent;
        app.style.load("res/styles/"+fileName+".style");
        app.update_style();
        return 0;
    } else {
        std::cout << "file not found: " << "res/styles/"+fileName+".style\n";
        return 1;
    }

}
