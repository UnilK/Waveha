#ifndef APP_MAINFRAME_H
#define APP_MAINFRAME_H

#include "ui/frame.h"


class MainFrame : ui::Frame{

protected:

public:

    MainFrame();
    MainFrame(Core *core_, Window *master_, std::map<std::string, std::string> values = {});

};

#endif
