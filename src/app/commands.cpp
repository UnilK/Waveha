#include "app/app.h"
#include "app/box.h"
#include "app/tab.h"

#include "wstream/wstream.h"

#include <iostream>
#include <fstream>

namespace app {

int32_t App::execute_command(ui::Command cmd){
    
    if(execute_standard(cmd)){
        return 0;
    } else {
        std::cout << "unrecognized command: " << cmd.address << ' ' << cmd.command << '\n';
    }

    return 1;
}

void App::log_command(ui::Command cmd){
    session.push_back(cmd);
}

SessionCommands::SessionCommands(){
    commandId = "ses";
    commandHelp = "save & load sessions";
    commandDocs = {
        {"save __name", "save session"},
        {"load _name", "load session"},
        {"new _name", "start new session"},
        {"rename _name", "rename session"}};
}

int32_t SessionCommands::execute_command(ui::Command cmd){
   
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
        if(!file.good()){
            return 1;
            std::cout << "bad file\n";
        }
        
        app.clean();

        app.sessionName = name;

        std::string mem;

        while(std::getline(file, mem)){
            ui::Command command = app.create_command(mem);
            app.deliver_address(command);
        }

        return 0;

    } else if(prefix == "new"){

        std::ifstream file;
        std::string name;

        cin >> name;
        
        if(name == "") return 1;

        app.clean();

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
    commandId = "box";
    commandHelp = "manage boxes";
    commandDocs = {
        {"analyzer _name", "create new analyzer"},
        {"place _box _tab", "place box to tab"},
        {"detach _box", "detach box from tab"}};
}

int32_t BoxCommands::execute_command(ui::Command cmd){

    if(execute_standard(cmd)) return 0;

    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;

    App &app = *(App*)commandParent;
        
    if(prefix == "analyzer"){
        
        std::string name;
        cin >> name;

        Analyzer *pointer = new Analyzer(&app.window, name);
        MainFrame &main = app.window.main;

        app.analyzers.push_back(pointer);
        app.window.main.add_box(pointer);
        app.analyzerCommands.link_child(pointer);
        if(main.chosenTab != nullptr) pointer->place_to_tab(main.chosenTab); 

        app.log_command(cmd);

        return 0;
    } else if(prefix == "place"){

        std::string box, tab;
        cin >> box >> tab;

        Box *boxp = app.window.main.find_box(box);
        Tab *tabp = app.window.main.find_tab(tab);

        if(boxp != nullptr && tabp != nullptr){
            
            boxp->place_to_tab(tabp);
            app.log_command(cmd);
            
            return 0;
        }
        else {
            if(boxp == nullptr) std::cout << "error: box not found\n";
            if(tabp == nullptr) std::cout << "error: tab not found\n";
            return 1;
        }
        
    } else if(prefix == "detach"){
        
        std::string box;
        cin >> box;

        Box *boxp = app.window.main.find_box(box);

        if(boxp != nullptr){
            boxp->detach_from_tab();
        }
        
        app.log_command(cmd);
        
        return 0;
    } else {
        std::cout << "command not recognized. try \"help\"\n";
        return 1;
    }

}



TabCommands::TabCommands(){
    commandId = "tab";
    commandHelp = "manage tabs";
    commandDocs = {
        {"new _name", "create new tab"}};
}

int32_t TabCommands::execute_command(ui::Command cmd){

    if(execute_standard(cmd)) return 0;

    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;
        
    App &app = *(App*)commandParent;
    
    if(prefix == "new"){
        
        std::string name;
        cin >> name;

        Tab *pointer = new Tab(&app.window, name);

        app.window.main.add_tab(pointer);
        app.window.main.chosenTab = pointer;

        app.log_command(cmd);

        return 0;
    } else {
        std::cout << "command not recognized. try \"help\"\n";
        return 1;
    }
}


AudioCommands::AudioCommands(){
    commandId = "aud";
    commandHelp = "manage audio";
    commandDocs = {
        {"link _file __handle", "link wav files"},
        {"list", "list linked audio"}};
}

int32_t AudioCommands::execute_command(ui::Command cmd){
    
    if(execute_standard(cmd)) return 0;

    App &app = *(App*)commandParent;
    
    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;

    if(prefix == "link"){

        std::string fileName, handle;

        cin >> fileName >> handle;

        iwstream I;
        I.logging = 1;
        
        if(I.open(fileName)){
            if(handle.empty()) handle = fileName;
            AudioSource source{fileName, handle, AudioSource::File};
            app.sources.push_back(source);
            app.log_command(cmd);
            return 0;
        } else if(I.open("audio/" + fileName + ".wav")){
            if(handle.empty()) handle = fileName;
            AudioSource source{"audio/" + fileName + ".wav", handle, AudioSource::File};
            app.sources.push_back(source);
            app.log_command(cmd);
            return 0;
        } else {
            std::cout << "error linking audio:\n";
            for(auto log : I.get_log()) std::cout << log << '\n';
            return 1;
        }

    } else if(prefix == "list"){

        std::cout << "audio sources:\n";
        for(auto i : app.sources) std::cout << i.handle << ": " << i.name << '\n'; 

    } else {

    }

    return 1;
}



AnalyzerCommands::AnalyzerCommands(){
    commandId = "ana";
    commandHelp = "analyze audio";
    commandDocs = {{"list", "list analyzers"}};
}

int32_t AnalyzerCommands::execute_command(ui::Command cmd){
    
    if(execute_standard(cmd)) return 0;

    App &app = *(App*)commandParent;
    
    std::stringstream cin(cmd.command);
    std::string prefix;

    cin >> prefix;
    
    if(prefix == "list"){
        std::cout << "analyzers:\n";
        for(Analyzer *a : app.analyzers) std::cout << a->get_title() << '\n';
        return 0;
    } else {
        std::cout << "analyzer command not recognized.\n";
        return 1;
    }
}

BlingCommands::BlingCommands(){
    commandId = "bling";
    commandHelp = "manage styles";
    commandDocs = {
        {"_name", "load syle file"}};
}

int32_t BlingCommands::execute_command(ui::Command cmd){
    
    if(execute_standard(cmd)) return 0;

    std::stringstream cin(cmd.command);
    std::string fileName;

    cin >> fileName;
    
    App &app = *(App*)commandParent;
    
    if(app.set_style("res/styles/"+fileName+".style")){
        std::cout << "file not found: res/styles/"+fileName+".style\n";
        return 1;
    } else {
        std::cout << "style updated\n";
        return 0;
    }

}

}
