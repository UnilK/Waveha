#pragma once

#include "ui/terminal.h"

#include <map>
#include <functional>
#include <fstream>

namespace app {

class Session;
class Loader;
class Saver;
class App;

class SessionDir : public ui::Directory {

public:

    SessionDir(Session&);

private:

    Session &session;

    void save(ui::Command);
    void load(ui::Command);
    void rename(ui::Command);
    void create_new(ui::Command);

};

class Session {

    // session manager class.

public:

    Session(App*);

    void save();
    void load(std::string file);

    void rename(std::string name_);
    void create_new(std::string name_);
    std::string get_name();

    bool valid_file(std::string file);

    SessionDir dir;
    
private:

    App &app;

    std::string name = "untitled";

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
    void write_bool(bool);
    void write_int(int);
    void write_unsigned(unsigned);
    void write_float(float);
    void write_double(double);
    void write_block(unsigned bytes, void *data);

    void close();

    bool bad();

private:

    std::ofstream out;

};



class Loader {

public:

    Loader(std::string file);

    std::string read_string();
    bool read_bool();
    int read_int();
    unsigned read_unsigned();
    float read_float();
    double read_double();
    std::vector<char> read_block();

    void close();

    bool bad();

private:

    std::ifstream in;

};

}

