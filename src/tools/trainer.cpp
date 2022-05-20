#include "tools/trainer.h"
#include "app/app.h"
#include "app/creations.h"
#include "ui/clock.h"

#include <iostream>

namespace app {

Trainer::Trainer(App *a) :
    Content(a),
    app(*a),
    trainer(&Trainer::train_on_thread, std::ref(*this)),
    slider(a, ui::Side::up, ui::Side::up, {.look = "basebox", .height = 100}),
    terminal(a, {.look = "baseterminal", .border = {0, 0, 0, 0}}),
    graph(a, {.look = "agraph", .border = {0, 0, 0, 0}})
{

    setup_grid(2, 1);
    fill_width({1});
    fill_height({1, 0});

    put(0, 0, &graph);
    put(1, 0, &slider);

    slider.set_border(0, 0 ,0, 0);
    slider.stack.create(1);
    slider.stack.put(&terminal, 0);
    slider.stack.update();
    slider.stack.fill_height({1, 0});
    slider.stack.set_border(0, 0, 0, 0);
    slider.functions.set_border(0, 0, 1, 1);
    graph.set_border(0, 0, 0, 0);

    terminal.put_directory("re", &app.creations.dir);
    
    terminal.erase_entry("cd");
    terminal.erase_entry("pwd");
    terminal.put_function("config", [&](ui::Command c){ config(c); });
    terminal.put_function("train", [&](ui::Command c){ train(c); });
    terminal.put_function("test", [&](ui::Command c){ test(c); });
    terminal.put_function("clean", [&](ui::Command c){ clean(c); });
    terminal.put_function("info", [&](ui::Command c){ info(c); });

    terminal.document("config", "[variable] [value] set training config variable");
    terminal.document("train", "switch training");
    terminal.document("test", "test stack");
    terminal.document("clean", "clear result data");
    terminal.document("info", "list the configuration");
}

Trainer::~Trainer(){
    alive = 0;
    trainer.join();
}

namespace Factory { extern std::string trainer; }
std::string Trainer::content_type(){ return Factory::trainer; }

void Trainer::save(ui::Saver &saver){
    saver.write_int(mode);
    saver.write_string(stackName);
    saver.write_string(dataName);
    saver.write_string(testName);
    saver.write_unsigned(bsize);
    saver.write_unsigned(tinterval);
    saver.write_double(speed);
    saver.write_double(decay);
}

void Trainer::load(ui::Loader &loader){
    mode = loader.read_int();
    stackName = loader.read_string();
    dataName = loader.read_string();
    testName = loader.read_string();
    bsize = loader.read_unsigned();
    tinterval = loader.read_unsigned();
    speed = loader.read_double();
    decay = loader.read_double();
    updatedFlag = 1;
}

void Trainer::on_tick(){
    if(updatedFlag) set_reconfig();
}

void Trainer::on_reconfig(){
    updatedFlag = 0;
    dataLock.lock();
    if(mode == 0){
        std::vector<float> points(results.size());
        for(unsigned i=0; i<points.size(); i++) points[i] = results[i].correct;
        graph.set_data(points);
        graph.fit_x();
        graph.fit_y();
        graph.set_reconfig();
    }
    else if(mode == 1){
        std::vector<float> points(results.size());
        for(unsigned i=0; i<points.size(); i++) points[i] = results[i].error;
        graph.set_data(points);
        graph.fit_x();
        graph.fit_y();
        graph.set_reconfig();
    }
    dataLock.unlock();
}

void Trainer::train(ui::Command c){
    running ^= 1;
}

void Trainer::config(ui::Command c){
   
    try {
        std::string var = c.pop();

        if(var == "data"){
            dataName = c.pop();
        }
        else if(var == "stack"){
            stackName = c.pop();
        }
        else if(var == "test"){
            testName = c.pop();
        }
        else if(var == "bsize"){
            unsigned n = std::stoul(c.pop());
            if(n > 0 && n < (1<<20)) bsize = n;
        }
        else if(var == "interval"){
            unsigned n = std::stoul(c.pop());
            if(n > 0 && n < (1<<20)) tinterval = n;
        }
        else if(var == "speed"){
            float n = std::stof(c.pop());
            speed = n;
        }
        else if(var == "decay"){
            float n = std::stof(c.pop());
            decay = n;
        }
        else if(var == "mode"){
            mode = std::stoi(c.pop());
            set_reconfig();
        }
        else {
            c.source.push_error("variable not recognized: " + var + "\n"
                    "options are: data, stack, test, bsize, interval, speed, mode");
        }
    }
    catch (const std::invalid_argument &e){
        c.source.push_error("sto_ parse error");
    }

}

void Trainer::test(ui::Command c){
    
    ml::Stack *stack = app.creations.get_stack(stackName);
    ml::TrainingData *data = app.creations.get_mldata(testName);
    
    if(data != nullptr && stack != nullptr){
        auto res = stack->test(*data);
        dataLock.lock();
        results.push_back(res);
        updatedFlag = 1;
        dataLock.unlock();
    }
}

void Trainer::clean(ui::Command c){
    dataLock.lock();
    results.clear();
    set_reconfig();
    dataLock.unlock();
}

void Trainer::save_stack(ui::Command c){
    // TODO
}

void Trainer::load_stack(ui::Command c){
    // TODO
}

void Trainer::info(ui::Command c){

    std::string message;
    message += "mode: " + std::to_string(mode) + "\n";
    
    message += "stack: " + stackName + " ";
    if(app.creations.get_stack(stackName)){
        if(app.creations.get_stack(stackName)->good()) message += "ok\n";
        else message += "broken :(\n";
    } else message += "not found\n";
    
    message += "data: " + dataName + " ";
    if(app.creations.get_mldata(dataName)) message += "ok\n";
    else message += "not found\n";
    
    message += "test: " + testName + " ";
    if(app.creations.get_mldata(testName)) message += "ok\n";
    else message += "not found\n";
    
    message += "batch size: " + std::to_string(bsize) + "\n";
    message += "test interval: " + std::to_string(tinterval) + "\n";
    message += "change speed: " + std::to_string(speed) + "\n";
    message += "decay factor: " + std::to_string(decay);
    c.source.push_output(message);
}

void Trainer::train_on_thread(){
    
    ui::Clock clock(1);
    
    unsigned count = 0;

    while(alive){

        if(!running){
            clock.join_tick();
            clock.force_sync_tick();
            continue;
        }

        count %= tinterval;
        if(count == 0){
            ml::Stack *stack = app.creations.get_stack(stackName);
            ml::TrainingData *data = app.creations.get_mldata(testName);
            if(data != nullptr && stack != nullptr){
                auto res = stack->test(*data);
                dataLock.lock();
                results.push_back(res);
                updatedFlag = 1;
                dataLock.unlock();
            }
        }
        
        ml::Stack *stack = app.creations.get_stack(stackName);
        ml::TrainingData *data = app.creations.get_mldata(dataName);
        
        if(data != nullptr && stack != nullptr){
            stack->train_program(*data, bsize, 1, speed, decay);
            count++;
        }
    }
}

}

