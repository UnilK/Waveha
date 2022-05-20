#pragma once

#include "ui/terminal.h"
#include "app/session.h"

#include <map>
#include <string>
#include <tuple>

namespace ml {

typedef std::pair<std::vector<float>, std::vector<float> > InputLabel;
typedef std::vector<InputLabel > TrainingData;
class Stack;

}

namespace app {

class App;
class Creations;

class CreationsDir : public ui::Directory {

public:

    CreationsDir(Creations&);

private:

    Creations &creations;
    
    void create_waves(ui::Command);

    void blur_mnist(ui::Command);
    void ft_mnist(ui::Command);

    void load_mldata(ui::Command);
    void remove_mldata(ui::Command);
    
    void list_stuff(ui::Command);
    
    void create_stack(ui::Command);
    void load_stack(ui::Command);
    void save_stack(ui::Command);
    void remove_stack(ui::Command);

};

class Creations : public Presistent {

public:

    Creations(App*);
    virtual ~Creations();

    void save(ui::Saver&);
    void load(ui::Loader&);
    void reset();

    int create_waves(std::string directory, std::string output, unsigned density);

    int blur_mnist(std::string name);
    int ft_mnist(std::string name);
    
    int load_mldata(std::string type, std::string name, std::string file);
    int remove_mldata(std::string name);
    
    int create_stack(std::string name, std::string file);
    int load_stack(std::string name, std::string file);
    int save_stack(std::string name, std::string file);
    int remove_stack(std::string name);

    ml::TrainingData *get_mldata(std::string name);
    ml::Stack *get_stack(std::string name);

    CreationsDir dir;
    friend class CreationsDir;

private:
    
    App &app;

    std::map<std::string, ml::Stack*> stacks;
    std::map<std::string, std::tuple<std::string, std::string, ml::TrainingData*> > datas;

};

}

