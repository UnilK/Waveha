#include "app/app.h"

#include <iostream>

namespace app {

App::App() :
    Window(800, 800, "waveha"),
    tools(this),
    terminal(this, {.look = "toolterminal"}),
    terminalBox(this, ui::Side::down, ui::Side::up, {.look = "tools", .height = 400}),
    layoutSlider(this, ui::Side::up, ui::Side::left, {.look = "tools"})
{

    setup_grid(1, 2);
    fill_width(1, 1);
    fill_height(0, 1);

    put(0, 0, &tools);
    put(0, 1, &layoutSlider);

    tools.stack.push_back(terminalBox.set_manager(&tools.stack));
    terminalBox.overload_inner(&terminal);
    terminalBox.set_label("terminal");

    terminal.set_border(0, 0, 0, 0);

    layoutSlider.set_slidable(0);

    terminalBox.set_buttons(1, 1, 1, 1, 1, 0);

    update_grid();
}

App::~App(){}

}
