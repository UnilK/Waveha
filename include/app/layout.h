#pragma once

#include "ui/slider.h"
#include "ui/box.h"
#include "app/session.h"

namespace app {

class App;
class Tab;
class Slot;

class Layout : public ui::Slider, public Presistent {

public:

    Layout(App*);
    virtual ~Layout();

    virtual void set_look(std::string look_);

    void save(Saver&);
    void load(Loader&);

    void select_slot(Slot*);
    void forget_slot();

private:
    
    App &app;
    
    Slot *selected;

    ui::Button addTab;
    
    void add_tab();

};

}

