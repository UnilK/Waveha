#pragma once

#include "ui/slider.h"
#include "ui/box.h"
#include "app/session.h"
#include "ui/terminal.h"

namespace app {

class App;
class Tab;
class Slot;
class Layout;

class LayoutDir : public ui::Directory {

public:

    LayoutDir(Layout&);

private:

    Layout &layout;

    void set_content(ui::Command);

};



class Layout : public ui::Slider, public Presistent {

    // layout management class.

public:

    Layout(App*);
    virtual ~Layout();

    virtual void set_look(std::string look_);

    void save(ui::Saver&);
    void load(ui::Loader&);
    void reset();

    void select_slot(Slot*);
    void forget_slot();

    Slot *get_selected();

    LayoutDir dir;

    friend class LayoutDir;

private:
    
    App &app;

    Slot *selected;

    ui::Button addTab;
    
    void add_tab();

};

}

