#pragma once

class Tab;

#include "app/resizableFrame.h"
#include "app/box.h"
#include "ui/text.h"

class Tab : public ResizableFrame {
    
public:

    Tab(ui::Frame *parent_, std::string title_, std::map<std::string, std::string> values = {});

    bool swap_box(int32_t index, int32_t direction);
    bool push_box(int32_t index, int32_t direction);

    void detach_box(int32_t index);
    bool add_box(Box *box);

    int32_t box_index(Box *box);

    int32_t on_event(sf::Event event);

    void rename(std::string name);
    std::string get_title();
    
    const int32_t boxMax = 25;

protected:

    std::string title;

    ui::Frame inner;
    ui::SolidFrame boxFrame;
    ui::Text titleFrame;

};

