#include "ui/text.h"

#include <SFML/Graphics/Text.hpp>

#include <sstream>

namespace ui{

int32_t Text::setup(std::map<std::string, std::string> &values){
    
    std::stringstream value;
    
    if(read_value("text", value, values)){
        text = "";
        std::string word;
        while(std::getline(value, word)) text += word+"\n";
        if(!text.empty()) text.pop_back();
    }
    
    return 0;
}

Text::Text(Frame *parent_, std::map<std::string, std::string> values) :
    ContentFrame(parent_, values)
{
    setup(values);
}

int32_t Text::draw(){

    initialize();
   
    Look l = core->style.get_look(look);

    canvas.clear(l.color("background"));

    sf::Text textBox(text, l.font("font"), l.num("text_size"));
    textBox.setFillColor(l.color("text_color"));
    
    sf::FloatRect rect = textBox.getGlobalBounds();
    
    float textX = canvasWidth / 2 - rect.width / 2;
    float textY = canvasHeight / 2 - rect.height;

    textBox.setPosition(textX, textY);

    canvas.draw(textBox);

    display();

    return 0;
}

}

