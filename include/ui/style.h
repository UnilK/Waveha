#pragma once

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics/Font.hpp>

namespace ui {

namespace Style {

    // poor man's css.

    /*
        .style file example:

        font
           default res/fonts/arial.ttf
        ;

        define
           green 00ff00
           red ea2909ee
        ;

        look

        basebutton
            borderThickness 2
            borderColor efefef
            textColor .red
            font default
        ;

        myButton
            pressed pressedButton
            normal normalButton
        ;
        
        normalButton
            .basebutton
            background .green
            textSize 20
        ;
        
        pressedButton
            .normalButton
            borderThickness 1
            font mono
            textSize 19
        ;

        ;
        
        font
           mono res/fonts/ubuntuMono.ttf
        ;


     */

    void load(std::string styleFile);

    std::map<std::string, std::string> &look(std::string key);
    sf::Font &font(std::string key);
}

class Styled {

    // simple class for accessing styles

public:

    Styled();
    virtual ~Styled();

    virtual void set_look(std::string look_);
    
    std::string chars(std::string key);
    sf::Color color(std::string key);
    sf::Font &font(std::string key);
    uint32_t textStyle(std::string key);
    long double num(std::string key);
    std::vector<long double> nums(std::string key);

protected:

    std::string look;

};

}

