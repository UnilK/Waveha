#pragma once

#include "ml/stack.h"

#include <map>
#include <string>

namespace app {

typedef std::vector<ml::InputLabel > TrainingData;

class Creations;

class CreationsDir : public ui::Directory {

public:

    CreationsDir(Creations&);

private:

    Creations &creations;

    void load_data(ui::Command);
    void remove_data(ui::Command);
    
    void list_stuff(ui::Command);
    void load_stack(ui::Command);
    void save_stack(ui::Command);
    void remove_stack(ui::Command);

};

class Creations {

public:

    Creations(App*);

    int load_data(std::string type, std::string file);
    int remove_data(std::string name);
    
    int load_stack(std::string name, std::string file);
    int save_stack(std::string name, std::string file);
    int remove_stack(std::string name);

    TrainingData *get_data(std::string name);
    ml::Stack *get_stack(std::string name);

    CreationsDir dir;
    friend class CreationsDir;

private:
    
    App &app;

    std::map<std::string, ml::Stack> stacks;
    std::map<std::string, TrainingData> datas;

};

}

