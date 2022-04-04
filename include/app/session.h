#pragma once

#include "ui/terminal.h"
#include "ui/fileio.h"

namespace app {

class Session;
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

    virtual void save(ui::Saver&);
    virtual void load(ui::Loader&);

};

}

