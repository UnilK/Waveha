#pragma once

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics/Font.hpp>

namespace ui{


class Style{

    // class for managing outlooks.

protected:

    std::map<std::string, sf::Font> fonts;
    std::map<std::string, std::map<std::string, std::string> > looks;

public:

    Style(std::string styleFile);
    
    std::map<std::string, std::string> &operator[](std::string key);
    sf::Font &font(std::string key);

};

}

