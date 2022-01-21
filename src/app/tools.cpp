#include "app/tools.h"

namespace app {

Tools::Tools(ui::Window *master_) :
    ui::Slider(master_, ui::Side::right, ui::Side::up, {.look = "tools", .width = 400}),
    dummy0(master_, ui::Side::down, ui::Side::up, {.look = "tools", .height = 200}),
    dummy1(master_, ui::Side::down, ui::Side::up, {.look = "tools", .height = 200}),
    dummy2(master_, ui::Side::down, ui::Side::up, {.look = "tools", .height = 200})
{
    set_label("tools");
    
    set_border(0, 0, 0, 0);
    stack.set_border(0, 0, 0, 0);
    functions.set_border(1, 1, 0, 0);

    dummy0.set_label("dummy0");
    dummy1.set_label("dummy1");
    dummy2.set_label("dummy2");

    stack.push_back(dummy0.set_manager(&stack));
    stack.push_back(dummy1.set_manager(&stack));
    stack.push_back(dummy2.set_manager(&stack));
}

}
