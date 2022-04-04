#pragma once

#include "ui/box.h"
#include "app/session.h"

namespace app {

class App;
class Slot;

class Tab : public ui::Box, public Presistent {

public:

    Tab(App*);
    virtual ~Tab();

    virtual void set_look(std::string look_);

    void save(ui::Saver&);
    void load(ui::Loader&);
    void reset();

    void func_detach();

private:
    
    App &app;
    
    ui::Button addSlot;

    void add_slot();

};

}

