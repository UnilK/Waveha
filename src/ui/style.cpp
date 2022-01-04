#include <ui/style.h>

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

namespace ui{

Style::Style(std::string styleFile){
    load(styleFile);
}

void Style::load(std::string styleFile){
    
    fonts.clear();
    looks.clear();
    macros.clear();
    
    std::ifstream I(styleFile);
   
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
                    std::string look = lines[i];
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

}

