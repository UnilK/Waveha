#ifndef APP_APP_H
#define APP_APP_H

#include "ui/core.h"
#include "ui/window.h"
#include "app/style.h"

class App : public ui::Core{
    
protected:

    ui::Window main;

public:

    App();

};

#endif
