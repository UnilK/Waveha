#include "app/layout.h"
#include "app/app.h"
#include "app/tab.h"
#include "app/slot.h"

namespace app {

// layout /////////////////////////////////////////////////////////////////////

Layout::Layout(App *a) :
    ui::Slider(a, ui::Side::up, ui::Side::left, {.look = "basebox"}),
    dir(*this),
    app(*a),
    addTab(a, [&](){ add_tab(); }, "+")
{
    set_border(0, 0, 0, 0);
    functions.set_border(0, 0, 0, 1);
    label.set_border(0, 0, 0, 1);
    stack.set_border(0, 0, 0, 0);
    set_slidable(0);

    addTab.set_border(1, 0, 0, 1);
    addTab.text_stick(ui::Text::middle);
    addTab.text_offset(0, -0.3);
    buttons.push_back(&addTab);

    set_look(look);
}

Layout::~Layout(){
    for(uint32_t i=0; i<stack.size(); i++)
        if(stack.get(i) != nullptr)
            delete stack.get(i);
}

void Layout::set_look(std::string look_){

    ui::Slider::set_look(look_);

    addTab.set_look(chars("buttons"));

    float size = num("barSize");

    addTab.set_target_size(size, size);
    
    buttons.update_grid();
    buttons.fit_content();
}

void Layout::save(Saver &saver){

}

void Layout::load(Loader &loader){

}

void Layout::select_slot(Slot *s){
    if(selected != s) forget_slot();
    selected = s;
}

void Layout::forget_slot(){
    if(selected != nullptr) selected->forget();
    selected = nullptr;
}

Slot *Layout::get_selected(){
    return selected;
}

void Layout::add_tab(){
    Tab *tab = new Tab(&app);
    stack.push_back(tab->set_manager(&stack));
}

// directory //////////////////////////////////////////////////////////////////

LayoutDir::LayoutDir(Layout &l) : layout(l) {
    put_function("set", [&](ui::Command c){ set_content(c); });
}

void LayoutDir::set_content(ui::Command c){
    c.source.push_output("not implemented yet.");
}

}
