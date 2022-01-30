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
    buttons.push_back(&addTab);

    set_look(look);
}

Layout::~Layout(){
    reset();
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
    
    saver.write_float(canvasX);

    saver.write_unsigned(stack.size());

    for(uint32_t i=0; i<stack.size(); i++){
        Tab *tab = (Tab*)stack.get(i);
        tab->save(saver);
    }
}

void Layout::load(Loader &loader){

    reset();

    set_canvas_position(loader.read_float(), 0);

    unsigned tamount = loader.read_unsigned();
    
    for(uint32_t i=0; i<tamount; i++){
        Tab *tab = new Tab(&app);
        stack.push_back(tab->set_manager(&stack));
        tab->load(loader);
    }

    update_grid();
}

void Layout::reset(){

    selected = nullptr;

    for(uint32_t i=0; i<stack.size(); i++) delete stack.get(i);

    stack.clear();

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

    document("set", "[type] create content to the currently selected slot."
            " Select a slot by clicking it.");
}

void LayoutDir::set_content(ui::Command c){
    
    if(layout.selected == nullptr){
        c.source.push_error("select a slot first by clicking one");
    }
    else {
        if(!layout.selected->content_from_type(c.pop())){
            c.source.push_error("content type not recognized");
        }
    }
}

}
