#include "app/session.h"
#include "app/app.h"
#include "app/audio.h"
#include "app/layout.h"
#include "app/creations.h"

#include <string>
#include <string.h>

namespace app {

// session ////////////////////////////////////////////////////////////////////

Session::Session(App *a) :
    dir(*this),
    app(*a)
{}

void Session::save(){
    
    ui::Saver saver("sessions/" + name + ".was");

    if(saver.bad()) return;

    app.audio.save(saver);
    app.creations.save(saver);
    app.layout.save(saver);

    saver.close();
}

void Session::load(std::string file){

    ui::Loader loader("sessions/" + file + ".was");

    if(loader.bad()) return;

    app.audio.load(loader);
    app.creations.load(loader);
    app.layout.load(loader);

    name = file;

    app.layout.set_label(name);

    loader.close();
}

void Session::rename(std::string name_){
    name = name_;
    app.layout.set_label(name);
}

void Session::create_new(std::string name_){
    name = name_;
    app.layout.reset();
    app.audio.reset();
    app.layout.set_label(name);
}

std::string Session::get_name(){ return name; }

bool Session::valid_file(std::string file){
    
    std::ifstream I("sessions/" + file + ".was");

    return I.good();
}

// directory //////////////////////////////////////////////////////////////////

SessionDir::SessionDir(Session &s) :
    session(s)
{
    put_function("save", [&](ui::Command c){ save(c); });
    put_function("load", [&](ui::Command c){ load(c); });
    put_function("name", [&](ui::Command c){ rename(c); });
    put_function("new", [&](ui::Command c){ create_new(c); });

    document("save", "{name} save current session");
    document("load", "[name] load a session from a file and discard the current one");
    document("name", "[new name] rename the current session");
    document("new", "[name] start a new session and discard the current one");
}

void SessionDir::save(ui::Command c){
    if(!c.empty()) rename(c);
    session.save();
}

void SessionDir::load(ui::Command c){
    
    std::string file = c.pop();

    if(session.valid_file(file)){
        session.load(file);
    }
    else {
        c.source.push_error("invalid source file: \"" + file + "\"");
    }
}

void SessionDir::rename(ui::Command c){
    
    std::string name = c.pop();

    if(!name.empty()){
        session.rename(name);
    }
    else {
        c.source.push_error("name must not be empty");
    }
}

void SessionDir::create_new(ui::Command c){
    
    std::string name = c.pop();

    if(!name.empty()){
        session.create_new(name);
    }
    else {
        c.source.push_error("new name must not be empty");
    }
}

// presistent /////////////////////////////////////////////////////////////////

void Presistent::save(ui::Saver&){}

void Presistent::load(ui::Loader&){}

}

