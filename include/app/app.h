#pragma once

namespace app { class App; }

#include "ui/core.h"
#include "ui/window.h"

#include "wave/file.h"

#include "app/mainFrame.h"
#include "app/analyzer.h"

#include <iostream>
#include <vector>

namespace app {

class SessionCommands : public ui::Commandable {
public:
    SessionCommands();
    int32_t execute_command(ui::Command cmd);
};

class BoxCommands : public ui::Commandable {
public:
    BoxCommands();
    int32_t execute_command(ui::Command cmd);
};

class TabCommands : public ui::Commandable {
public:
    TabCommands();
    int32_t execute_command(ui::Command cmd);
};

class AudioCommands : public ui::Commandable {
public:
    AudioCommands();
    int32_t execute_command(ui::Command cmd);
};

class AnalyzerCommands : public ui::Commandable {
public:
    AnalyzerCommands();
    int32_t execute_command(ui::Command cmd);
};

class BlingCommands : public ui::Commandable {
public:
    BlingCommands();
    int32_t execute_command(ui::Command cmd);
};



class AudioSource {
public:
    enum Type {
        File
    };
    std::string name, handle;
    Type type;
};


class App : public ui::Core {

public:

    App();
    ~App();

    void clean();

    int32_t execute_command(ui::Command cmd);

    int32_t update_children();

    friend class FileCommands;

    MainWindow window;

    SessionCommands sessionCommands;
    BoxCommands boxCommands;
    TabCommands tabCommands;
    AudioCommands audioCommands;
    AnalyzerCommands analyzerCommands;
    BlingCommands blingCommands;

    std::string sessionName = "untitled";
    std::vector<ui::Command> session;
    void log_command(ui::Command cmd);

    wave::Source *create_source(std::string handle);
    std::vector<AudioSource> sources;
    
    std::vector<Analyzer*> analyzers;

};

}
