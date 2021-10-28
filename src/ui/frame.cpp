#include "ui/frame.h"

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <sstream>
#include <algorithm>
#include <math.h>

namespace ui{
    
TextureFrame::TextureFrame(const sf::Texture &tex_, float wwpos_, float whpos_) : tex(tex_){
    wwpos = wwpos_;
    whpos = whpos_;
    wpos = hpos = 0;
    sf::Vector2<uint32_t> size = tex.getSize();
    width = size.x;
    height = size.y;
}

Frame::Frame(){}

Frame::Frame(Core *core_, Window *master_, std::map<std::string, std::string> values){ 
    core = core_;
    master = master_;
    parent = nullptr;
    setup(values);
}

Frame::Frame(Frame *parent_, std::map<std::string, std::string> values){
    parent = parent_;
    core = parent->core;
    master = parent->master;
    setup(values);
}

int32_t Frame::setup(std::map<std::string, std::string> values){
    
    if(values["look"] != "") look = values["look"];
    if(values["width"] != "") std::stringstream(values["width"]) >> width;
    if(values["height"] != "") std::stringstream(values["height"]) >> height;
    if(values["wpos"] != "") std::stringstream(values["wpos"]) >> wpos;
    if(values["hpos"] != "") std::stringstream(values["hpos"]) >> hpos;
    if(values["rowSpan"] != "") std::stringstream(values["rowSpan"]) >> rowSpan;
    if(values["columnSpan"] != "") std::stringstream(values["columnSpan"]) >> columnSpan;

    return 1;
}
        
int32_t Frame::event_update(sf::Event){
    
    return 1;
}

int32_t Frame::coreapp_update(){

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr) grid[i][j]->coreapp_update();
        }
    }

    return 1;
}

Frame *Frame::find_focus(){

    float x = master->mwpos;
    float y = master->mhpos;

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            float wbegin = wpos+wwpos+rmax[i];
            float wend = wpos+wwpos+rmax[std::min(rows, i+grid[i][j]->rowSpan)];
            float hbegin = hpos+whpos+cmax[j];
            float hend = hpos+whpos+cmax[std::min(columns, j+grid[i][j]->columnSpan)];
            if(wbegin <= x && x <= wend && hbegin <= y && y <= hend){
                return grid[i][j]->find_focus(); 
            }
        }
    }

    return this;
}

int32_t Frame::send_texture(TextureFrame &tex){
   
    // cut out the parts that don't fit in the frame and send upwards if necessary.

    tex.wpos = std::max(tex.wpos, wwpos-tex.wwpos);
    tex.hpos = std::max(tex.hpos, whpos-tex.whpos);
    tex.width = std::min(tex.width, wwpos+wwidth-tex.wwpos-tex.wpos);
    tex.height = std::min(tex.height, whpos+wheight-tex.whpos-tex.hpos);
    
    if(refreshFlag){
        textures.push_back(tex);
    } else {
        if(parent == this || parent == nullptr){
            master->send_texture(tex);
        } else {
            parent->send_texture(tex);
        }
    }

    return 1;
}

int32_t Frame::refresh(){

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr) grid[i][j]->refresh();
        }
    }

    if(!refreshFlag) return 1;

    Look s = core->style.get(look);

    sf::RenderTexture window;
    window.create(wwidth, wheight);
    window.clear(s.color("bg"));

    for(TextureFrame &tex : textures){
        if(tex.height >= 0.5f && tex.width >= 0.5f){
            sf::Rect<int32_t> rect(
                    std::round(tex.wpos), std::round(tex.hpos),
                    std::round(tex.width), std::round(tex.height));
            sf::Sprite sprite(tex.tex, rect);
            sprite.setPosition(wwpos-tex.wwpos+tex.wpos, whpos-tex.whpos+tex.hpos);
            window.draw(sprite);
        }
    }

    TextureFrame texf(window.getTexture(), wwpos, whpos);
    textures.clear();
    refreshFlag = 0;

    send_texture(texf);

    return 1;
}

int32_t Frame::setup_grid(int32_t rows_, int32_t columns_){
    
    if(rows_ < 0 || columns_ < 0) return 0;
    delete_grid();
    
    rows = rows_;
    columns = columns;
    
    grid.resize(rows);
    rfill.resize(rows, 0);
    cfill.resize(columns, 0);
    rmax.resize(rows+1, 0);
    cmax.resize(columns+1, 0);
    for(int32_t i=0; i<rows; i++){
        grid[i] = std::vector<Frame*>(columns, nullptr);
    }
    return 1;
}

int32_t Frame::delete_grid(){
    columns = 0;
    rows = 0;
    rfill.clear();
    cfill.clear();
    grid.clear();
    return 1;
}

int32_t Frame::update_grid(int32_t direction){

    std::fill(rmax.begin(), rmax.end(), 0);
    std::fill(cmax.begin(), cmax.end(), 0);
    
    // calculate grid dimensions. Each row & column size is determined by the most
    // space consuming frame that ends there.
    for(int32_t i=0; i<rows; i++){
        if(i) rmax[i] = std::max(rmax[i], rmax[i-1]);
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr){
                int32_t end = std::min(rows, i+grid[i][j]->rowSpan);
                rmax[end] = std::max(rmax[end], rmax[i]+grid[i][j]->height);
            }
        }
    }
    
    for(int32_t j=0; j<columns; j++){
        if(j) cmax[j] = std::max(cmax[j], cmax[j-1]);
        for(int32_t i=0; i<rows; i++){
            if(grid[i][j] != nullptr){
                int32_t end = std::min(columns, j+grid[i][j]->columnSpan);
                cmax[end] = std::max(cmax[end], cmax[j]+grid[i][j]->width);
            }
        }
    }

    // enough space for all child frames,
    // use all space provided by the window
    cwidth = std::max(std::max(width, cmax[columns]), wwidth);
    cheight = std::max(std::max(height, rmax[rows]), wheight);

    // calculate extra space that needs to be used.
    float rextra = std::max(0.0f, cmax[columns]-cwidth);
    float cextra = std::max(0.0f, rmax[rows]-cheight);

    wpos = std::max(wpos, wwidth-cwidth);
    hpos = std::max(hpos, wheight-cheight);

    float rtotal = 0, ctotal = 0;
    for(float i : rfill) rtotal += i;
    for(float i : cfill) ctotal += i;
    if(rtotal == 0.0f) rtotal = 1.0f;
    if(ctotal == 0.0f) ctotal = 1.0f;

    float rsum = 0, csum = 0;

    // use all space
    for(int32_t i=0; i<rows; i++){
        rmax[i] += rsum;
        rsum += rextra*(rfill[i]/rtotal);
    } rmax[rows] += rsum;
    
    for(int32_t i=0; i<columns; i++){
        cmax[i] += columns;
        csum += cextra*(cfill[i]/ctotal);
    } cmax[columns] += csum;

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            float wbegin = std::round(wpos+wwpos+cmax[i]);
            float wend = std::round(wpos+wwpos+cmax[std::min(columns, i+grid[i][j]->columnSpan)]);
            float hbegin = std::round(hpos+whpos+rmax[j]);
            float hend = std::round(hpos+whpos+rmax[std::min(rows, j+grid[i][j]->rowSpan)]);
            grid[i][j]->wwpos = wbegin;
            grid[i][j]->whpos = hbegin;
            grid[i][j]->wwidth = wend-wbegin;
            grid[i][j]->wheight = hend-hbegin;
            if(direction&1) grid[i][j]->update_grid(1);
        }
    }

    if(direction&2 && parent != this && parent != nullptr) parent->update_grid(2);

    refreshFlag = 1;
    
    return 1;
}

int32_t Frame::config_row(std::vector<float> rfill_){
   rfill = rfill_;
   rfill.resize(std::max(rows, (int32_t)rfill.size()), 0);
   return 1;
}

int32_t Frame::config_column(std::vector<float> cfill_){
   cfill = cfill_;
   cfill.resize(std::max(columns, (int32_t)cfill.size()), 0);
   return 1;
}

int32_t Frame::config_row(int32_t row, float value){
    rfill.resize(row+1, 0);
    rfill[row] = value;
    return 1;
}

int32_t Frame::config_column(int32_t column, float value){
    cfill.resize(column+1, 0);
    cfill[column] = value;
    return 1;
}

}

