#include <ui/style.h>

#include <SFML/Graphics/Text.hpp>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace ui {

typedef std::map<std::string, std::string> kvpairs;

namespace Style {

std::map<std::string, sf::Font> fonts;
std::map<std::string, kvpairs > looks;
kvpairs macros;

void load(std::string styleFile){
   
    if(styleFile.empty()) return;

    fonts.clear();
    looks.clear();
    macros.clear();
    
    std::ifstream I(styleFile);

    if(!I.good()) return;
   
    std::string line;
    std::vector<std::string> lines;
    while(std::getline(I, line)) lines.push_back(line);

    int n = lines.size();

    std::map<std::string, std::vector<std::string> > edges_out, edges_in;
    
    for(int i=0; i<n; i++){

        if(lines[i] == "font"){
            
            i++;
            
            for(;i<n && lines[i][0] != ';'; i++){
                
                if(lines[i].empty()) continue;
                
                std::string key, value;
                std::stringstream in(lines[i]);
                in >> key;
                std::getline(in, value);

                unsigned j = 0;
                while(j < value.size() && value[j] == ' ') j++;                    
                if(j != value.size()) value = value.substr(j, value.size()-j);

                fonts[key].loadFromFile(value);
            }
        }
        else if(lines[i] == "look"){
            
            i++;
            
            for(;i<n && lines[i][0] != ';'; i++){
                
                if(lines[i].empty()) continue;
                
                std::string look = lines[i++];
                edges_in[look] = {};

                for(;i<n && lines[i][0] != ';'; i++){
                    
                    if(lines[i].empty()) continue;
                    
                    std::string key, value;
                    std::stringstream in(lines[i]);
                    in >> key;
                    std::getline(in, value);

                    if(key[0] == '.'){
                        
                        edges_out[key.substr(1, key.size() - 1)].push_back(look);
                        edges_in[look].push_back(key.substr(1, key.size() - 1));

                    }
                    else {

                        unsigned j = 0;
                        while(j < value.size() && value[j] == ' ') j++;                    
                        if(j != value.size()) value = value.substr(j, value.size()-j);

                        looks[look][key] = value;
                    }
                }
            }
        } else if(lines[i] == "define"){
            
            i++;
            
            for(;i<n && lines[i][0] != ';'; i++){
                
                if(lines[i].empty()) continue;
                
                std::string key, value;
                std::stringstream in(lines[i]);
                in >> key;
                std::getline(in, value);
                        
                unsigned j = 0;
                while(j < value.size() && value[j] == ' ') j++;                    
                if(j != value.size()) value = value.substr(j, value.size()-j);

                macros["."+key] = value;
            }
        } else if(!lines[i].empty()){
            std::cout << "style parameter \"" + lines[i] + "\"not recognized.\n"
                "must be font, look or define";
        }
    }
    
    for(auto &look : looks){
        for(auto &i : look.second){
            if(!i.second.empty() && i.second[0] == '.'){
                i.second = macros[i.second];
            }
        }
    }

    // serach for "topological sorting" for some algorithm reference.

    std::vector<std::string> order;
    std::map<std::string, int> count;

    for(auto i : edges_in){
        count[i.first] = i.second.size();
        if(i.second.size() == 0) order.push_back(i.first);
    }

    for(unsigned i=0; i<order.size(); i++){
        
        std::string look = order[i];
        
        kvpairs original = looks[look];

        for(auto inherited : edges_in[look]){
            for(auto kv : looks[inherited]){
                looks[look][kv.first] = kv.second;
            }
        }
        
        for(auto kv : original){
            looks[look][kv.first] = kv.second;
        }

        for(auto derived : edges_out[look]){
            if(--count[derived] == 0){
                order.push_back(derived);
            }
        }
    }


    I.close();
}

kvpairs &look(std::string key){
    return looks[key];
}

sf::Font &font(std::string key){
    return fonts[key];
}

}



Styled::Styled(){}

Styled::~Styled(){}

void Styled::set_look(std::string look_){
    look = look_;
}

std::string Styled::chars(std::string key){
    return Style::look(look)[key];
}

sf::Color Styled::color(std::string key){

    std::string color = chars(key);
    
    if(color.size() == 6) color += "ff";
    while(color.size() < 8) color.push_back('0');
    
    unsigned rgba;
    std::stringstream(color) >> std::hex >> rgba >> std::dec;

    return sf::Color(rgba>>24&0xff, rgba>>16&0xff, rgba>>8&0xff, rgba&0xff);
}

sf::Font &Styled::font(std::string key){
    return Style::font(chars(key));
}

long double Styled::num(std::string key){
    long double x = 0;

    if(!chars(key).empty())
        std::stringstream(chars(key)) >> x;
    
    return x;
}

std::vector<long double> Styled::nums(std::string key){
    
    std::vector<long double> v;
    std::stringstream cin(chars(key));

    long double x;
    while(cin >> x) v.push_back(x);

    return v;
}

unsigned Styled::textStyle(std::string key){
    
    std::string styles = chars(key), style;

    std::stringstream cin(styles) ;

    unsigned result = 0;

    while(cin >> style){
        if(style == "bold") result |= sf::Text::Style::Bold;
        else if(style == "italic") result |= sf::Text::Style::Italic;
        else if(style == "underlined") result |= sf::Text::Style::Underlined;
        else if(style == "strikethrough") result |= sf::Text::Style::StrikeThrough;
    }

    return result;
}

}
