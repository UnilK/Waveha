#include "app/mainFrame.h"

#include <iostream>

namespace app {

TabBar::TabBar(ui::Window *master_, ui::Frame *tabFrame_, kwargs values) :
    ui::SolidFrame(master_, values),
    tabFrame(tabFrame_)
{}

int32_t TabBar::on_event(sf::Event event, int32_t priority){
    
    if(priority > 0) return -1;

    if(event.type == sf::Event::MouseWheelScrolled){
            
        float scrollSpeed = 50;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            scrollSpeed = 1;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            scrollSpeed = 100;
        
        float deltaX = scrollSpeed * event.mouseWheelScroll.delta;

        tabFrame->set_canvas_position(
                std::max(0.0f, tabFrame->canvasX + deltaX),
                tabFrame->canvasY);
        tabFrame->update_grid();

        return 1;
    }

    return -1;
}


MainFrame::MainFrame(ui::Window *master_, kwargs values) :
    ui::Frame(master_, values),
    tabFrame(master_, kwargs{{"look", "MainFrame"}}),
    tabBar(master_, &tabFrame, kwargs{{"look", "TabBar"}, {"height", "15"}}),
    infoTab(master_, "info")
{
    look = "MainFrame";

    setup_grid(2, 1);
    fill_width(0, 1);
    fill_height(1, 1);

    put(0, 0, &tabBar);
    put(1, 0, &tabFrame);

    tabFrame.autoContain = 0;
    tabFrame.setup_grid(1, tabMax + 1);
    tabFrame.fill_height(0, 1);

    tabFrame.put(0, 0, &infoTab);
    infoTab.targetWidth = 200;

    update_grid();
}

void MainFrame::clean(){
    
    chosenTab = nullptr;

    for(Tab *tab : tabs) delete tab;
    tabs.clear();
    boxes.clear();
    
    tabFrame.setup_grid(1, tabMax + 1);
    tabFrame.fill_height(0, 1);
    tabFrame.put(0, 0, &infoTab);
    infoTab.targetWidth = 200;

    update_grid();
}

bool MainFrame::add_tab(Tab *tab){

    if((int32_t)tabs.size() == tabMax) return 0;

    tabs.push_back(tab);
    for(int32_t i=1; i<=tabMax; i++){
        if(tabFrame.get(0, i) == nullptr){
            tabFrame.place_frame(0, i, tab);
            break;
        }
    }

    return 1;
}

bool MainFrame::delete_tab(std::string name){
    
    for(uint32_t i=1; i<=tabs.size(); i++){
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

}
