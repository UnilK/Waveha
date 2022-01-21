#include "app/tools.h"
#include "app/app.h"

namespace app {

Tools::Tools(App *master_) :
    ui::Slider(master_, ui::Side::right, ui::Side::up, {.look = "tools", .width = 400}),
    app(*master_)
{
    set_label("tools");
    
    set_border(0, 0, 0, 0);
    stack.set_border(0, 0, 0, 0);
    functions.set_border(1, 1, 0, 0);
}

}
