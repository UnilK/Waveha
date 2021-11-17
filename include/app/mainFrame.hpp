#pragma once

#include <iostream>
#include "ui/frame.h"
#include "ui/button.h"

class MainFrame;
class MainWindow;

class ToolBar : public ui::SolidFrame{

public:

    ToolBar(Frame *parent_, std::map<std::string, std::string> values = {}) :
        ui::SolidFrame(parent_, values)
    {    
        look = "ToolBar";
    }

};

class Switch : public ui::Button {

protected:

    bool color = 0;

public:

    Switch(ui::Frame *parent_, std::map<std::string, std::string> values = {}) :
        ui::Button(parent_, Switch::swap, this, values)
    {
        look = "redSwitch";
    }
    
    static int32_t swap(void *frame){

        auto &f = *(Switch*)frame;

        f.color ^= 1;
        if(f.color) f.look = "blueSwitch";
        else f.look = "redSwitch";
        f.refreshFlag = 1;
        return 0;
    }


};

class MainView : public ui::SolidFrame{

public:

    MainView(Frame *parent_, std::map<std::string, std::string> values = {}) :
        ui::SolidFrame(parent_, values)
    {

        look = "MainView";

        setup_grid(5, 5);

        for(int i=0; i<5; i++){
            for(int j=0; j<5; j++){
                grid[i][j] = new Switch(this, {
                        {"width", "200"},
                        {"height", "200"},
                        {"border", "10 20 10 20"},
                        {"pad", "10 10 10 40"}});
            }
        }

    }

};

class SideBar : public ui::SolidFrame{

public:

    SideBar(Frame *parent_, std::map<std::string, std::string> values = {}) :
        ui::SolidFrame(parent_, values)
    {
        look = "SideBar";
    }

};

class MainFrame : public ui::SolidFrame {

public:
    
    MainView mainView;
    ToolBar toolBar; 
    SideBar sideBar;
    
    MainFrame(ui::Window *master_, std::map<std::string, std::string> values = {}) :
        
        ui::SolidFrame(master_, values),
        
        mainView(this, {
                {"width", "1000"},
                {"height", "700"},
                {"fill", "0.0 1 0.0  0.0 1 0.0"}}),
        
        toolBar(this, {{"height", "40"}}),
        
        sideBar(this, {{"width", "200"}})
    {

        look = "MainFrame";
        setup_grid(2, 2);

        fill_width({1, 0});
        fill_height({0, 1, 0});

        grid[0][0] = &toolBar;
        toolBar.set_span(1, 2);

        grid[1][0] = &mainView;
        
        grid[1][1] = &sideBar;

        update_grid();
    }
};

class MainWindow : public ui::Window{
    
public: 

    MainFrame mainFrame;

    MainWindow(ui::Core *core_) : 
        ui::Window(core_, 1200, 800, "Waveha"),
        mainFrame(this)
    {
        mainframe = &mainFrame;
    }

    int32_t on_close(){
        return core->stop();
    }

};

