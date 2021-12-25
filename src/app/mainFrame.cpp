#include "app/mainFrame.h"

#include <iostream>

MainFrame::MainFrame(ui::Window *master_, std::map<std::string, std::string> values) :
    ui::SolidFrame(master_, values)
{
    look = "MainFrame";
    update_grid();
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

