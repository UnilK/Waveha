#include "app/mainFrame.h"

#include <iostream>


TabBar::TabBar(ui::Frame *parent_, ui::Frame *tabFrame_, std::map<std::string, std::string> values) :
    ui::SolidFrame(parent_, values),
    tabFrame(tabFrame_)
{
    look = "TabBar";
}

int32_t TabBar::on_event(sf::Event event, int32_t priority){
    
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseWheelScrolled){
        
        float deltaX = 50 * event.mouseWheelScroll.delta;

        tabFrame->move_canvas(deltaX, 0);
        tabFrame->update_grid();

        return 1;
    }

    return -1;
}


MainFrame::MainFrame(ui::Window *master_, std::map<std::string, std::string> values) :
    ui::Frame(master_, values),
    tabFrame(this, {{"look", "MainFrame"}}),
    tabBar(this, &tabFrame, {{"height", "15"}})
{
    look = "MainFrame";

    setup_grid(2, 1);
    fill_width(0, 1);
    fill_height(1, 1);

    grid[0][0] = &tabBar;
    grid[1][0] = &tabFrame;

    tabBar.setup_grid(1, tabMax);
    tabBar.fill_width(0, 1);

    tabFrame.setup_grid(1, tabMax);
    tabFrame.fill_height(0, 1);

    update_grid();
}

bool MainFrame::add_tab(Tab *tab){

    if((int32_t)tabs.size() == tabMax) return 0;

    tabs.push_back(tab);
    for(int32_t i=0; i<tabMax; i++){
        if(tabFrame.grid[0][i] == nullptr){

            tabFrame.place_frame(0, i, tab);
            break;
        }
    }

    return 1;
}

bool MainFrame::delete_tab(std::string name){
    for(uint32_t i=0; i<tabs.size(); i++){
        if(tabs[i]->get_title() == name){

            tabFrame.remove_frame(tabs[i]);
            for(auto j : boxes) if(j->get_tab() == tabs[i]) j->detach_from_tab();
            
            delete tabs[i];
            tabs.erase(tabs.begin() + i);

            return 1;
        }
    }
    return 0;
}

Tab *MainFrame::find_tab(std::string name){
    Tab *tab = nullptr;
    for(auto i : tabs) if(i->get_title() == name) tab = i;
    return tab;
}

bool MainFrame::add_box(Box *box){
    boxes.push_back(box);
    return 1;
}

bool MainFrame::delete_box(std::string name){
    for(uint32_t i=0; i<boxes.size(); i++){
        if(boxes[i]->get_title() == name){
            boxes[i]->detach_from_tab();
            delete boxes[i];
            boxes.erase(boxes.begin() + i);
            return 1;
        }
    }
    return 0;
}

Box *MainFrame::find_box(std::string name){
    Box *box = nullptr;
    for(auto i : boxes) if(i->get_title() == name) box = i;
    return box;
}



MainWindow::MainWindow(ui::Core *core_) : 
    ui::Window(core_, 1200, 800, "Waveha", 120),
    main(this)
{
    mainframe = &main;
}

int32_t MainWindow::on_close(){
    return core->stop();
}

