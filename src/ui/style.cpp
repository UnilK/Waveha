#include <ui/style.h>

#include <sstream>
#include <iomanip>

namespace ui{

Look::Look(){}

Look::Look(std::map<std::string, std::string> values_){
    values = values_;
}

void Look::set(std::string key, std::string value){
    values[key] = value;
}

std::string Look::get(std::string key){
    return values[key];
}

sf::Color Look::color(std::string key){

    std::string color = values[key];
    while(color.size() < 8) color.push_back('f');
    uint8_t r, g, b, a;
    std::stringstream(color.substr(0, 2)) >> std::hex >> r;
    std::stringstream(color.substr(2, 2)) >> std::hex >> g;
    std::stringstream(color.substr(4, 2)) >> std::hex >> b;
    std::stringstream(color.substr(6, 2)) >> std::hex >> a;

    return sf::Color(r, g, b, a);
}


Style::Style(){}

Style::Style(std::map<std::string, Look> looks_){
    looks = looks_;
}

void Style::set(std::string key, Look &value){
    looks[key] = value;
}

Look Style::get(std::string key){
    return looks[key];
}

}

