#pragma once

#include <string>
#include <map>
#include <SFML/Graphics.hpp>

namespace ui{

class Look{
    
    // class for managing the look of frames.        

    protected:

        std::map<std::string, std::string> values;
    
    public:

        Look();
        Look(std::map<std::string, std::string> values_);

        void set(std::string key, std::string value);
        std::string get(std::string key);
        sf::Color color(std::string key);

};

class Style{

    // class for managing looks.

    protected:
        
        std::map<std::string, Look> looks;

    public:

        Style();
        Style(std::map<std::string, Look> looks_);

        void set(std::string key, Look &value);
        Look get(std::string key);

};

}

