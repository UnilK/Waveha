#pragma once

#include "ui/box.h"
#include "app/session.h"
#include "app/tab.h"

namespace app {

class App;
class Tab;
class Content;

class Slot : public ui::Box, public Presistent {

public:

    Slot(Tab*);
    virtual ~Slot();

    virtual void set_look(std::string look_);
    
    void save(ui::Saver&);
    void load(ui::Loader&);
    void reset();

    void set_content(Content*);

    void func_detach();

    Capture on_event(sf::Event event, int priority);

    void select();
    void forget();

    bool content_from_type(std::string type);

private:

    App &app;
    Tab *tab;

    Content *content = nullptr;

    ui::Button pushLeft, pushRight;
    
    void push_left();
    void push_right();

};

}

