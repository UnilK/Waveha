#pragma once

#include "ui/slider.h"
#include "ui/box.h"
#include "app/session.h"

#include <functional>

namespace app {

class App;
class Tab;
class Slot;
class Content;

class Layout : public ui::Slider, public Presistent {

public:

    Layout(App*);
    virtual ~Layout();

    void save(Saver&);
    void load(Loader&);

    void forget_slot();

private:
    
    Slot *selected;

    std::vector<Tab*> tabs;

    ui::Button addTab;
    
    void add_tab();

};



class Tab : public ui::Box, public Presistent {

public:

    Tab(App*);
    virtual ~Tab();

    void save(Saver&);
    void load(Loader&);

    void func_detach();

private:
    
    std::vector<Slot*> slots;

    ui::Button addSlot;

    void add_slot();

};



class Slot : public ui::Box, public Presistent {

public:

    Slot(App*);
    virtual ~Slot();
    
    void save(Saver&);
    void load(Loader&);

    void set_content(Content*);

    void func_detach();

    ui::Frame::Capture on_event(sf::Event event, int32_t priority);

    void select();
    void forget();

    static void add_content_type(std::string type, std::function<Content*(void)>);

private:

    ui::Button pushLeft, pushRight;
    
    void push_left();
    void push_right();

    Content *content_from_type(std::string type);

    static std::map<std::string, std::function<Content*(void)> > types;

};



class Content : public Presistent {

public:

    Content(std::string);
    virtual ~Content();

    std::string get_type();

private:

    std::string type;

};

}
