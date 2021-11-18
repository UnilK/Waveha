#pragma once

#include <string>
#include <map>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>

namespace ui{

class Style;

class Look{
    
    // class for managing the look of frames.        

protected:

    std::map<std::string, std::string> values;

public:

    Style *master = nullptr;
    
    Look();
    Look(std::map<std::string, std::string> values_);

    std::string chars(std::string key);
    sf::Color color(std::string key);
    sf::Font &font(std::string key);
    long double num(std::string key);

};

class Style{

    // class for managing looks.

protected:

    std::map<std::string, sf::Font> fonts;
    std::map<std::string, Look> looks;

public:

    Style();
    Style(
            std::map<std::string, std::string> fonts_,
            std::map<std::string, Look> looks_);

    
    void set_look(std::string key, Look value);
    void set_font(std::string key, std::string value);

    Look get_look(std::string key);
    sf::Font &get_font(std::string key);

};

}

