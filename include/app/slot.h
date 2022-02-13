#pragma once

#include "ui/box.h"
#include "app/session.h"
#include "app/tab.h"

#include <map>
#include <functional>

namespace app {

class App;
class Tab;

class Content : public ui::Frame, public Presistent {

public:

    Content(App*, ui::Kwargs = {});

    // static std::string type;
    virtual std::string content_type() = 0;

    // !!!  Slot::add_content_type must be called for each class
    // use a patter something like:
    // .h : static int init_class;
    // .cpp :
    //      XX::init_class = [](){
    //          Slot::add_content_type(type, [](App *a){ return new XX(a); } );
    //          return 0;
    //      }();
    // (yuck!)

};

class Slot : public ui::Box, public Presistent {

public:

    Slot(Tab*);
    virtual ~Slot();

    virtual void set_look(std::string look_);
    
    void save(Saver&);
    void load(Loader&);
    void reset();

    void set_content(Content*);

    void func_detach();

    Capture on_event(sf::Event event, int priority);

    void select();
    void forget();

    bool content_from_type(std::string type);
    
    static bool valid_type(std::string type);
    static void add_content_type(std::string type, std::function<Content*(App*)>);

private:

    App &app;
    Tab *tab;

    Content *content = nullptr;

    ui::Button pushLeft, pushRight;
    
    void push_left();
    void push_right();

    static std::map<std::string, std::function<Content*(App*)> > types;

};

}

