#pragma once

class Box;

#include "ui/button.h"
#include "ui/text.h"

#include "app/resizableFrame.h"
#include "app/tab.h"

class Box : public ResizableFrame {

public:

    Box(ui::Frame *parent_, std::string title_, std::map<std::string, std::string> values = {});

    bool place_to_tab(Tab *newTab);
    void detach_from_tab();

    void rename(std::string name);

    static int32_t switch_up(void *box);
    static int32_t switch_down(void *box);
    static int32_t detach_box(void *box);

    std::string get_title();
    Tab *get_tab();

protected:

    ui::Frame inner, buttonFrame;
    
    ui::Text titleText;
    ui::Button upSwitch, downSwitch, xButton;

    Tab *tab = nullptr;
    int32_t index();

    std::string title;

};

class NoContentBox : public Box {

public:

    NoContentBox(ui::Frame *parent_, std::string title_, std::map<std::string, std::string> values = {});

    ui::Text text;

};

