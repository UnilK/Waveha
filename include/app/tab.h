#pragma once

namespace app { class Tab; }

#include "app/resizableFrame.h"
#include "app/box.h"
#include "ui/text.h"

namespace app {

class Tab : public ResizableFrame {
    
    /*
       style:
       -
       kwargs:
       -
     */

public:

    Tab(ui::Window *master_, std::string title_, std::map<std::string, std::string> values = {});
    Tab(ui::Frame *parent_, std::string title_, std::map<std::string, std::string> values = {});

    bool swap_box(int32_t index, int32_t direction);

    void detach_box(int32_t index);
    bool add_box(Box *box);
    void clean_layout();

    int32_t box_index(Box *box);

    int32_t on_event(sf::Event event, int32_t priority);

    void rename(std::string name);
    std::string get_title();
    
    const int32_t boxMax = 100;

protected:

    std::string title;

    ui::Frame inner;
    ui::SolidFrame boxFrame;
    ui::Text titleFrame;

};

}
