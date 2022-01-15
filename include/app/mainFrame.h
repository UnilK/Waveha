#pragma once

#include "ui/frame.h"
#include "app/tab.h"
#include "app/box.h"

namespace app {

using ui::Kwargs;

class TabBar : public ui::SolidFrame {

    /*
       style:
       -
     */

public:

    TabBar(ui::Window *master_, ui::Frame *tabFrame_, Kwargs = {});

    Capture on_event(sf::Event event, int32_t priority = 0);

    ui::Frame *tabFrame;

};



class MainFrame : public ui::Frame {
    
    /*
       style:
       -
     */

public:

    MainFrame(ui::Window *master_, Kwargs = {});

    bool add_tab(Tab *tab);
    bool delete_tab(std::string name);
    Tab *find_tab(std::string name);

    bool add_box(Box *box);
    bool delete_box(std::string name);
    Box *find_box(std::string name);
    
    const int32_t tabMax = 100;

    Tab *chosenTab = nullptr;

    std::vector<Tab*> tabs;
    std::vector<Box*> boxes;

    ui::SolidFrame tabFrame;
    TabBar tabBar;
    Tab infoTab;

    void clean();

};



class MainWindow : public ui::Window{

public:
    
    MainWindow(ui::Core *core_);
    
    int32_t on_close();
    
    MainFrame main;

};

}
