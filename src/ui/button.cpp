#include "ui/button.h"

namespace ui{
    
Button::Button(Frame *parent_, int32_t (*function_)(),
            std::map<std::string, std::string> values) :
    ContentFrame(parent_, values),
    function(function_)
{
    
}

int32_t Button::draw(){
    
    return 0;
}

int32_t Button::event_update(sf::Event){
    
    return 0;
}

}

