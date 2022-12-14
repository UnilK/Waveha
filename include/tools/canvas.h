#pragma once

#include "app/content.h"
#include "tools/graph.h"
#include "ui/slider.h"

#include <vector>
#include <map>

namespace app {

class App;
class CanvasTool;

class Canvas : public Graph {

public:

    Canvas(CanvasTool&, ui::Kwargs = {});

    void set_look(std::string look_);
    Capture on_event(sf::Event event, int priority);
    void on_refresh();

    void on_tick();

    void update_info();

    friend class CanvasTool;

private:

    CanvasTool &tool;
    sf::VertexArray upLine, downLine, sideLine;
    sf::Text brushT, brushS, brushM, cursorX, cursorY;

    bool brushing = 0;
    float xPos = 0, yPos = 0;
    double upHeight = 1, downHeight = 1;

};

class CanvasTool : public Content {

public:

    CanvasTool(App*);
    ~CanvasTool();
    
    std::string content_type();
    
    void save(ui::Saver&);
    void load(ui::Loader&);

    void name_output(ui::Command);
    void set_size(ui::Command);
    void config_brush(ui::Command);
    void nil_data(ui::Command);
    void switch_cyclic(ui::Command);
    void config_lines(ui::Command);
    void preset(ui::Command);
    void info(ui::Command);

    void draw(double x, double y);
    void update_data();

    void newdo();
    void undo();
    void redo();

    friend class Canvas;

private:

    App &app;

    std::string output = "untitled";

    std::vector<std::string> brushes = {"packet", "damp", "avg"};
    std::string brush = "packet";

    unsigned frequency = 0;
    float phase = 0, brushSize = 16, magnitude = 0.01;

    const unsigned MIN_SIZE = 16;
    const unsigned HISTORY_SIZE = 1<<7;

    bool cyclic = 1;
    int position = 0, length = 1<<10;
    std::vector<float> data;
    std::vector<std::vector<float> > undoh, redoh;

    void handle_history();
    
    ui::Slider slider;
    ui::Terminal terminal;
    Canvas canvas;

};

}

