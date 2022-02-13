#include "tools/meditor.h"

#include <math.h>
#include <algorithm>

namespace app {

Meditor::Meditor(App *a) : 
    Content(a),
    matrix(60)
{}

void Meditor::save(Saver&){}
void Meditor::load(Loader&){}

std::string Meditor::content_type(){ return type; }

const std::string Meditor::type = "medit";

int Meditor::init_class = [](){
    Slot::add_content_type(type, [](App *a){ return new Meditor(a); });
    return 0;
}();

void Meditor::resize_matrix(ui::Command){}
void Meditor::shuffle_matrix(ui::Command){}
void Meditor::unit_matrix(ui::Command){}

void Meditor::update_data(){}
void Meditor::set_begin(unsigned){}
void Meditor::set_end(unsigned){}

}
