#pragma once

#include "ui/frame.h"

namespace ui{

class Button : public ui::ContentFrame {
    
protected:

    std::string text;
    float borderLeft = 0, borderRight = 0, borderUp = 0, borderDown = 0;
    
    int32_t (*function)(void*);
    void *target = nullptr;
    bool pressed = 0;

private:

    int32_t setup(std::map<std::string, std::string> &values);

public:

    Button(Frame *parent_, int32_t (*function_)(void*), void *target_,
            std::map<std::string, std::string> values = {});

    int32_t draw();
    int32_t event_update(sf::Event);

};

}

