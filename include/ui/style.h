#pragma once

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics/Font.hpp>

namespace ui {

typedef std::map<std::string, std::string> kvpairs;

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

        myButton
            pressed pressedButton
            normal normalButton
        ;
        
        pressedButton
            background $green
            borderThickness 2
            borderColor efefef
            font default
            textSize 20
        ;
        
        pressedButton
            background $red
            borderThickness 1
            borderColor aaaaaa
            font mono
            textSize 19
        ;

        ;
        
        font
           mono res/fonts/ubuntuMono.ttf
        ;


     */

    void load(std::string styleFile);

    kvpairs &look(std::string key);
    sf::Font &font(std::string key);
}

class Styled {

    // simple class for accessing styles

public:

    Styled();
    virtual ~Styled();

    virtual int32_t set_look(std::string look_);
    
    std::string look;
    
    std::string chars(std::string key);
    sf::Color color(std::string key);
    sf::Font &font(std::string key);
    uint32_t textStyle(std::string key);
    long double num(std::string key);
};

}

