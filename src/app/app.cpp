#include "app/app.h"

#include <iostream>

namespace app {

App::App() :
    Window(800, 800, "waveha"),
    tools(this),
    temp(this, {.look = "temp"})
{

    setup_grid(1, 2);
    fill_width(1, 1);
    fill_height(0, 1);

    put(0, 0, &tools);
    put(0, 1, &temp);

    update_grid();
}

App::~App(){}

}
