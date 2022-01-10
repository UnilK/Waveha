#pragma once

#include "ui/frame.h"
#include "app/tab.h"
#include "app/box.h"

namespace app {

class TabBar : public ui::SolidFrame {

    /*
       style:
       -
       kwargs:
       -
     */

public:

    TabBar(ui::Frame *parent_, ui::Frame *tabFrame_, kwargs values = {});

    int32_t on_event(sf::Event event, int32_t priority = 0);

    ui::Frame *tabFrame;

};



class MainFrame : public ui::Frame {
    
    /*
       style:
       -
       kwargs:
       -
     */

public:

    MainFrame(ui::Window *master_, kwargs values = {});

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
