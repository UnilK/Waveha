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
    while(color.size() < 8) color.push_back('0');
    
    uint32_t rgba;
    std::stringstream(color) >> std::hex >> rgba >> std::dec;


    return sf::Color(rgba>>24&0xff, rgba>>16&0xff, rgba>>8&0xff, rgba&0xff);
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

