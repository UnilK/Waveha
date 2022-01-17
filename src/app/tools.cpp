#include "app/tools.h"

namespace app {

Tools::Tools(ui::Window *master_) :
    ui::Slider(master_, ui::Side::right, ui::Side::up, 1,
            {.look = "tools", .width = 400}),
    dummy(master_, ui::Side::down, ui::Side::up, 0, {.look = "tools", .height = 200})
{
    set_label("tools");
    
    set_border(0, 0, 0, 0);
    stack.set_border(0, 0, 0, 0);
    functions.set_border(1, 1, 0, 0);

    dummy.set_label("dummy");

    stack.push_back(&dummy);
}

}
