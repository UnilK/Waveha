#pragma once

#include "ui/frame.h"

namespace ui{

class Text : public ContentFrame {

    /*
       style parameters:
       background (color)
       textSize (num)
       textColor (color)
       font (font)
    */

public:

    Text(Frame *parent_, std::map<std::string, std::string> values = {});

    int32_t draw();

protected:

    std::string text = "";

private:

    int32_t setup(std::map<std::string, std::string> &values);

};

}

