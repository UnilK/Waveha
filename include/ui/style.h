#pragma once

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics/Font.hpp>

typedef std::map<std::string, std::string> kwargs;

namespace ui{

class Style{

    // class for managing outlooks.

    /*
        .style file template:

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

public:

    Style(std::string styleFile);
    
    void load(std::string styleFile);

    kwargs &operator[](std::string key);
    sf::Font &font(std::string key);

protected:

    std::map<std::string, sf::Font> fonts;
    std::map<std::string, kwargs > looks;
    kwargs macros;

};

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

