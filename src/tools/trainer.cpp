#include "tools/trainer.h"

#include "app/app.h"

namespace app {

Trainer::Trainer(App *a) :
    Content(a),
    app(*a),
    slider(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    graph(a, {.look = "agraph", .border = {0, 0, 0, 0}})
{

    setup_grid(2, 1);
    fill_width({1});
    fill_height({1, 0});

    put(0, 0, &graph);
    put(1, 0, &slider);

    slider.stack.create(1);
    slider.stack.put(&terminal, 1);
    slider.stack.update();
    slider.stack.fill_height({1, 0});
    slider.stack.set_border(0, 0, 0, 0);

    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("train", [&](ui::Command c){ train(c); });
    terminal.put_function("test", [&](ui::Command c){ test(c); });

    terminal.document("train", "[stack] [data] train stack with data");
    terminal.document("test", "[stack] [data] test stack with data");
}

void Trainer::train(ui::Command c){

}

void Trainer::test(ui::Command c){

}


}

