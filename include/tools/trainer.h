#pragma once

#include "app/content.h"
#include "tools/graph.h"
#include "ml/stack.h"
#include "ui/terminal.h"
#include "ui/slider.h"

#include <vector>
#include <thread>
#include <mutex>

namespace app {

class App;

class Trainer : public Content {

public:

    Trainer(App*);
    ~Trainer();
    
    std::string content_type();

    void save(ui::Saver&);
    void load(ui::Loader&);
    
    void on_tick();
    void on_reconfig();

private:

    void train(ui::Command);
    void config(ui::Command);
    void test(ui::Command);
    void clean(ui::Command);

    App &app;

    bool alive = 1, running = 0;
    std::thread trainer;
    std::mutex trainLock, dataLock;
    void train_on_thread();

    // config
    std::string stackName = "", dataName = "", testName = "";
    unsigned bsize = 100, tinterval = 100;
    double speed = 0.01;
    
    int mode = 0;
    bool updatedFlag = 0;
    std::vector<ml::Stack::TestAnalysis> results;

    ui::Slider slider;
    ui::Terminal terminal;
    
    Graph graph;

};

}

