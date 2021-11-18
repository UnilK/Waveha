#include <ui/style.h>

#include <iostream>
#include <sstream>
#include <iomanip>

namespace ui{

Look::Look(){}
Look::Look(std::map<std::string, std::string> values_) : values(values_) {}

std::string Look::chars(std::string key){
    return values[key];
}

sf::Color Look::color(std::string key){

    std::string color = values[key];
    while(color.size() < 8) color.push_back('0');
    
    uint32_t rgba;
    std::stringstream(color) >> std::hex >> rgba >> std::dec;

    return sf::Color(rgba>>24&0xff, rgba>>16&0xff, rgba>>8&0xff, rgba&0xff);
}

sf::Font &Look::font(std::string key){
    return master->get_font(values[key]);
}

long double Look::num(std::string key){
    long double x;
    std::stringstream(values[key]) >> x;
    return x;
}



Style::Style(){}
Style::Style(
        std::map<std::string, std::string> fonts_,
        std::map<std::string, Look> looks_){
    
    for(auto [k, v] : fonts_){
        fonts[k] = sf::Font();
        fonts[k].loadFromFile(v);
    }

    looks = looks_;
    for(auto &kv : looks) kv.second.master = this;

}

void Style::set_look(std::string key, Look value){
    looks[key] = value;
    looks[key].master = this;
}

void Style::set_font(std::string key, std::string value){
    fonts[key] = sf::Font();
    fonts[key].loadFromFile(value);
}

Look Style::get_look(std::string key){
    return looks[key];
}

sf::Font &Style::get_font(std::string key){
    return fonts[key];
}

}

