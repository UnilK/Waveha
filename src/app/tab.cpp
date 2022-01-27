#include "app/tab.h"
#include "app/slot.h"
#include "app/layout.h"
#include "app/app.h"

namespace app {

Tab::Tab(App *a) :
    Box(a, ui::Side::right, ui::Side::up, {.look = "basebox"}),
    app(*a),
    addSlot(a, [&](){ add_slot(); }, "+")
{
    addSlot.set_border(1, 1, 1, 0);
    addSlot.text_stick(ui::Text::middle);
    addSlot.text_offset(0, -0.3);
    buttons.insert(&addSlot, 0);

    set_look(look);
}

Tab::~Tab(){
    for(uint32_t i=0; i<stack.size(); i++)
        if(stack.get(i) != nullptr)
            delete stack.get(i);
}

void Tab::set_look(std::string look_){

    ui::Box::set_look(look_);

    addSlot.set_look(chars("buttons"));

    float size = num("barSize");

    addSlot.set_target_size(size, size);
    
    buttons.update_grid();
    buttons.fit_content();
}

void Tab::save(Saver &saver){

}

void Tab::load(Loader &loader){

}

void Tab::func_detach(){
    Box::func_detach();
    delete this;
}

void Tab::add_slot(){
    Slot *slot = new Slot(this);
    stack.push_back(slot->set_manager(&stack));
}

}

