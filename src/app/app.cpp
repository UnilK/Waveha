#include "app/app.h"

#include <iostream>

namespace app {

App::App() :
    Window(800, 800, "waveha"),
    audio(this),
    session(this),
    creations(this),
    layout(this),
    tools(this),
    terminal(this, {.look = "baseterminal"}),
    terminalBox(this, ui::Side::down, ui::Side::up, {.look = "basebox", .height = 400})
{

    setup_grid(1, 2);
    fill_width(1, 1);
    fill_height(0, 1);

    put(0, 0, &tools);
    put(0, 1, &layout);

    layout.set_label(session.get_name());

    tools.stack.push_back(terminalBox.set_manager(&tools.stack));
    terminalBox.overload_inner(&terminal);
    terminalBox.set_label("terminal");

    terminal.set_border(0, 0, 0, 0);

    terminalBox.set_buttons(1, 1, 1, 1, 1, 0);

    terminal.put_directory("la", &layout.dir);
    terminal.put_directory("au", &audio.dir);
    terminal.put_directory("sa", &session.dir);
    terminal.put_directory("re", &creations.dir);

    update_grid();
}

App::~App(){}

}
