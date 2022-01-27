#pragma once

#include <map>
#include <functional>
#include <fstream>

namespace app {

class Loader;
class Saver;
class App;

class Session {

    // session manager class.

public:

    Session(App*);

    void save();
    void load(std::string file);

    void rename(std::string name_);
    void create_new(std::string name_);
    
private:

    App *app;

    std::string name;

};



class Presistent {

public:

    virtual void save(Saver&);
    virtual void load(Loader&);

};



class Saver {

public:

    Saver(std::string file);

    void write_string(std::string);
    void write_int(int);
    void write_float(float);
    void write_block(int bytes, void *data);

    void close();

    bool bad();

private:

    std::ofstream out;

};



class Loader {

public:

    Loader(std::string file);

    std::string read_string();
    int read_int();
    float read_float();
    std::vector<char> read_block();

    void close();

    bool bad();

private:

    std::ifstream in;

};

}

