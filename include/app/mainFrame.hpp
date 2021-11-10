#pragma once

#include <iostream>
#include "ui/frame.h"

class MainFrame;
class MainWindow;

class ToolBar : public ui::Frame{

public:

    ToolBar() {}
    ToolBar(MainFrame *parent_,
            std::map<std::string, std::string> values = {}) : ui::Frame((Frame*)parent_, values){
        
        setup({{"look", "ToolBar"}});
        setup_grid(1, 1);

    }

};

class MainView : public ui::Frame{

public:

    MainView();
    MainView(MainFrame *parent_,
            std::map<std::string, std::string> values = {}) : ui::Frame((Frame*)parent_, values){

        setup({{"look", "MainView"}});
        setup_grid(1, 1);

    }

};

class SideBar : public ui::Frame{

public:

    SideBar();
    SideBar(MainFrame *parent_,
            std::map<std::string, std::string> values = {}) : ui::Frame((Frame*)parent_, values){

        setup({{"look", "SideBar"}});
        setup_grid(1, 1);

    }

};

class MainFrame : public ui::Frame{

public:
    
    ToolBar toolBar;
    MainView mainView;
    SideBar sideBar;

    MainFrame();
    MainFrame(ui::Window *master_, std::map<std::string, std::string> values = {}) :
        ui::Frame(master_, values),
        toolBar(this, {{"height", "40"}}),
        mainView(this),
        sideBar(this, {{"width", "200"}}){

        setup({{"look", "MainFrame"}});
        setup_grid(2, 2);

        update_grid(3);
    }
};

class MainWindow : public ui::Window{
    
public: 

    MainWindow(ui::Core *core_) : ui::Window(core_, {
            {"title", "waveha"},
            {"width", "1200"},
            {"height", "800"}}){

        mainframe = (ui::Frame*)new MainFrame(this); 
    }

    int32_t on_close(sf::Event event){
        return core->stop();
    }

};

