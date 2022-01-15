#pragma once

#include "ui/window.h"
#include "app/tab.h"

namespace app {

using ui::Kwargs;

class TabBar : public ui::SolidFrame {

public:

    TabBar(ui::Window *master_, ui::Frame *tabFrame_, Kwargs = {});

    Capture on_event(sf::Event event, int32_t priority = 0);

    ui::Frame *tabFrame;

};



class App : public ui::Window {

public:

    App();
    ~App();

private:

    ui::SolidFrame tabFrame;
    TabBar tabBar;
    Tab infoTab;

};

}
