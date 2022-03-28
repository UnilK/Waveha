#pragma once

#include "app/content.h"
#include "tools/graph.h"
#include "ml/stack.h"
#include "ui/terminal.h"
#include "ui/slider.h"

#include <vector>

namespace app {

class App;

class Trainer : public Content {

public:

    Trainer(App*);

private:
    
    void train(ui::Command);
    void test(ui::Command);

    App &app;

    ui::Slider slider;
    ui::Terminal terminal;
    
    Graph graph;

};

}

