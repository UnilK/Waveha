#include "app/app.h"

#include <iostream>

namespace app {

App::App() :
    Window(800, 800, "waveha"),
    tabFrame(this, {.look = "TabBackground"}),
    tabBar(this, &tabFrame, {.look = "TabBar", .height = 15}),
    infoTab(this, "info")
{
    setup_grid(2, 1);
    fill_width(0, 1);
    fill_height(1, 1);

    put(0, 0, &tabBar);
    put(1, 0, &tabFrame);

    tabFrame.auto_contain(0);
    tabFrame.setup_grid(1, 2);
    tabFrame.fill_height(0, 1);

    tabFrame.put(0, 0, &infoTab);
    infoTab.targetWidth = 200;

    update_grid();
}

App::~App(){}



TabBar::TabBar(ui::Window *master_, ui::Frame *tabFrame_, Kwargs kwargs) :
    ui::SolidFrame(master_, kwargs),
    tabFrame(tabFrame_)
{}

ui::Frame::Capture TabBar::on_event(sf::Event event, int32_t priority){
    
    if(priority > 0) return Capture::pass;

    if(event.type == sf::Event::MouseWheelScrolled){
            
        float scrollSpeed = 50;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) scrollSpeed = 1;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) scrollSpeed = 100;
        
        float deltaX = scrollSpeed * event.mouseWheelScroll.delta;

        tabFrame->set_canvas_position(
                std::max(0.0f, tabFrame->canvasX + deltaX),
                tabFrame->canvasY);
        tabFrame->update_grid();

        return Capture::capture;
    }

    return Capture::pass;
}

}
