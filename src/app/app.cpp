#include "app/app.h"

#include <iostream>

namespace app {

App::App() :
    Core("res/styles/default.style", 1000),
    window(this)
{

    commandId = "";

    link_child(&sessionCommands);
    link_child(&boxCommands);
    link_child(&tabCommands);
    link_child(&audioCommands);
    link_child(&analyzerCommands);
    link_child(&blingCommands);

    commandHelp = "Root directory.";
    commandDocs = {{"none", "none"}};

}

App::~App(){
    clean();
}

void App::clean(){
    
    std::string sessionName = "untitled";

    session.clear();
    sources.clear();

    window.main.clean();
    
    analyzerCommands.clear_children();
    for(Analyzer *a : analyzers) delete a;
    analyzers.clear();
}

wave::ReadableAudio *App::create_source(std::string handle){
   
    for(auto source : sources){
        if(source.handle == handle){
            if(source.type == AudioSource::File){
                wave::ReadableAudio *pointer = new wave::AudioFile(source.name);
                return pointer;
            }
        }
    }

    return nullptr;
}

}
