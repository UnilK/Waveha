#include <ui/style.h>
#include <ui/core.h>

#include <SFML/Graphics/Text.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace ui{

Style::Style(std::string styleFile){
    load(styleFile);
}

void Style::load(std::string styleFile){
   
    if(styleFile.empty()) return;

    fonts.clear();
    looks.clear();
    macros.clear();
    
    std::ifstream I(styleFile);

    if(!I.good()) return;
   
    std::string line;
    std::vector<std::string> lines;
    while(std::getline(I, line)) lines.push_back(line);
    
    int32_t n = lines.size();
    
    for(int32_t i=0; i<n; i++){

        if(lines[i] == "font"){
            i++;
            for(;i<n && lines[i][0] != ';'; i++){
                if(!lines[i].empty()){
                    std::string key, value;
                    std::stringstream in(lines[i]);
                    in >> key;
                    std::getline(in, value);

                    uint32_t j = 0;
                    while(j < value.size() && value[j] == ' ') j++;                    
                    if(j != value.size()) value = value.substr(j, value.size()-j);

                    fonts[key].loadFromFile(value);
                }
            }
        }
        else if(lines[i] == "look"){
            i++;
            for(;i<n && lines[i][0] != ';'; i++){
                if(!lines[i].empty()){
                    std::string look = lines[i++];
                    for(;i<n && lines[i][0] != ';'; i++){
                        if(!lines[i].empty()){
                            std::string key, value;
                            std::stringstream in(lines[i]);
                            in >> key;
                            std::getline(in, value);

                            uint32_t j = 0;
                            while(j < value.size() && value[j] == ' ') j++;                    
                            if(j != value.size()) value = value.substr(j, value.size()-j);

                            looks[look][key] = value;
                        }
                    }
                }
            }
        } else if(lines[i] == "define"){
            i++;
            for(;i<n && lines[i][0] != ';'; i++){
                if(!lines[i].empty()){
                    std::string key, value;
                    std::stringstream in(lines[i]);
                    in >> key;
                    std::getline(in, value);
                            
                    uint32_t j = 0;
                    while(j < value.size() && value[j] == ' ') j++;                    
                    if(j != value.size()) value = value.substr(j, value.size()-j);

                    macros["$"+key] = value;
                }
            }
        } else if(!lines[i].empty()){
            std::cout << "style parameter \"" + lines[i] + "\"not recognized.\n"
                "must be font, look or define";
        }
    }
    
    for(auto &look : looks){
        for(auto &i : look.second){
            if(!i.second.empty() && i.second[0] == '$'){
                i.second = macros[i.second];
            }
        }
    }


    I.close();
}

kwargs &Style::operator[](std::string key){
    return looks[key];
}

sf::Font &Style::font(std::string key){
    return fonts[key];
}


Styled::Styled(){}

Styled::~Styled(){}

int32_t Styled::set_look(std::string look_){
    look = look_;
    return 0;
}

std::string Styled::chars(std::string key){
    return Core::object->style[look][key];
}

sf::Color Styled::color(std::string key){

    std::string color = chars(key);
    
    if(color.size() == 6) color += "ff";
    while(color.size() < 8) color.push_back('0');
    
    uint32_t rgba;
    std::stringstream(color) >> std::hex >> rgba >> std::dec;

    return sf::Color(rgba>>24&0xff, rgba>>16&0xff, rgba>>8&0xff, rgba&0xff);
}

sf::Font &Styled::font(std::string key){
    return Core::object->style.font(chars(key));
}

long double Styled::num(std::string key){
    long double x = 0;

    if(!chars(key).empty())
        std::stringstream(chars(key)) >> x;
    
    return x;
}

uint32_t Styled::textStyle(std::string key){
    
    std::string styles = chars(key), style;

    std::stringstream cin(styles) ;

    uint32_t result = 0;

    while(cin >> style){
        if(style == "bold") result |= sf::Text::Style::Bold;
        else if(style == "italic") result |= sf::Text::Style::Italic;
        else if(style == "underlined") result |= sf::Text::Style::Underlined;
        else if(style == "strikethrough") result |= sf::Text::Style::StrikeThrough;
    }

    return result;
}

}

