#include "ui/frame.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

namespace ui{
    
TextureFrame::TextureFrame(const sf::Texture *tex_, float wwpos_, float whpos_) : tex(tex_){
    wwpos = wwpos_;
    whpos = whpos_;
    wpos = hpos = 0;
    sf::Vector2<uint32_t> size = tex->getSize();
    width = size.x;
    height = size.y;
}

Frame::Frame(){
    window = new sf::RenderTexture();
}

Frame::Frame(Window *master_, std::map<std::string, std::string> values){ 
    window = new sf::RenderTexture();
    master = master_;
    core = master->core;
    parent = nullptr;
    setup(values);
}

Frame::Frame(Frame *parent_, std::map<std::string, std::string> values){
    window = new sf::RenderTexture();
    parent = parent_;
    core = parent->core;
    master = parent->master;
    setup(values);
}

Frame::~Frame(){ delete window; }

int32_t Frame::setup(std::map<std::string, std::string> values){
    
    if(values["look"] != "") look = values["look"];
    if(values["width"] != "") std::stringstream(values["width"]) >> twidth;
    if(values["height"] != "") std::stringstream(values["height"]) >> theight;
    if(values["columnSpan"] != "") std::stringstream(values["columnSpan"]) >> columnSpan;
    if(values["rowSpan"] != "") std::stringstream(values["rowSpan"]) >> rowSpan;

    return 0;
}
        
int32_t Frame::event_update(sf::Event){
    
    return 0;
}

int32_t Frame::coreapp_update(){

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr) grid[i][j]->coreapp_update();
        }
    }

    return 0;
}

Frame *Frame::find_focus(){

    float x = master->mwpos;
    float y = master->mhpos;

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            float wbegin = cwpos+wwpos+hmax[i];
            float wend = cwpos+wwpos+hmax[std::min(rows, i+grid[i][j]->rowSpan)];
            float hbegin = chpos+whpos+wmax[j];
            float hend = chpos+whpos+wmax[std::min(columns, j+grid[i][j]->columnSpan)];
            if(wbegin <= x && x <= wend && hbegin <= y && y <= hend){
                return grid[i][j]->find_focus(); 
            }
        }
    }

    return this;
}

int32_t Frame::send_texture(TextureFrame &tex){
   
    // cut out the parts that don't fit in the frame and send upwards if necessary.

    tex.wpos -= std::min(0.0f, tex.wpos+tex.wwpos+cwpos-wwpos);
    tex.hpos -= std::min(0.0f, tex.hpos+tex.whpos+chpos-whpos);
    tex.width = std::min(tex.width, (wwpos+wwidth)-(tex.wpos+tex.wwpos+cwpos));
    tex.height = std::min(tex.height, (whpos+wheight)-(tex.hpos+tex.whpos+chpos));
   
    if(refreshFlag){
        textures.push_back(tex);
    } else {
        if(parent == this || parent == nullptr){
            master->send_texture(tex);
        } else {
            parent->send_texture(tex);
        }
    }

    return 0;
}

int32_t Frame::refresh(){

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr) grid[i][j]->refresh();
        }
    }

    if(!refreshFlag) return 0;

    Look s = core->style.get(look);

    window->clear(s.color("background"));

    for(TextureFrame &tex : textures){
        if(tex.height >= 0.5f && tex.width >= 0.5f){
            sf::Rect<int32_t> rect(
                    std::round(tex.wpos), std::round(tex.hpos),
                    std::round(tex.width), std::round(tex.height));
            sf::Sprite sprite(*tex.tex, rect);
            sprite.setPosition(
                    (tex.wwpos+tex.wpos-wwpos+cwpos),
                    wheight-tex.height-(tex.whpos+tex.hpos-whpos+chpos));
            window->draw(sprite);
        }
    }

    TextureFrame texf(&window->getTexture(), wwpos, whpos);
    textures.clear();
    refreshFlag = 0;

    send_texture(texf);

    return 0;
}

int32_t Frame::set_window_size(float wwidth_, float wheight_){
    wwidth = wwidth_;
    wheight = wheight_;
    if(wwidth*wheight > 0) window->create(wwidth, wheight);
    else window->create(1, 1);
    return 0;
}

int32_t Frame::set_window_position(float wwpos_, float whpos_){
    wwpos = wwpos_;
    whpos = whpos_;
    return 0;
}

int32_t Frame::set_target_dimensions(float twidth_, float theight_){
    twidth = twidth_;
    theight = theight_;
    return 0;
}

float Frame::target_width(){ return twidth+lpad+rpad; }

float Frame::target_height(){ return theight+upad+dpad; }

int32_t Frame::setup_grid(int32_t rows_, int32_t columns_){
    
    if(rows_ <= 0 || columns_ <= 0) return 1;
    
    rows = rows_;
    columns = columns_;
    
    grid = std::vector<std::vector<Frame*> >(rows, std::vector<Frame*>(columns, nullptr));
    hfill.resize(rows, 0);
    wfill.resize(columns, 0);
    hmax.resize(rows+1, 0);
    wmax.resize(columns+1, 0);
    
    return 0;
}

int32_t Frame::update_grid(int32_t direction){

    std::fill(hmax.begin(), hmax.end(), 0);
    std::fill(wmax.begin(), wmax.end(), 0);
   
    hmax[0] = upad;
    wmax[0] = lpad;

    // calculate grid dimensions. Each row & column size is determined by the most
    // space consuming frame that ends there.
    for(int32_t i=0; i<rows; i++){
        if(i) hmax[i] = std::max(hmax[i], hmax[i-1]);
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr){
                int32_t end = std::min(rows, i+grid[i][j]->rowSpan);
                hmax[end] = std::max(hmax[end], hmax[i]+grid[i][j]->target_height());
            }
        }
    }
    
    for(int32_t j=0; j<columns; j++){
        if(j) wmax[j] = std::max(wmax[j], wmax[j-1]);
        for(int32_t i=0; i<rows; i++){
            if(grid[i][j] != nullptr){
                int32_t end = std::min(columns, j+grid[i][j]->columnSpan);
                wmax[end] = std::max(wmax[end], wmax[j]+grid[i][j]->target_width());
            }
        }
    }

    // enough space for all child frames,
    // use all space provided by the window
    cwidth = std::max({target_width(), wmax[columns]+rpad, wwidth});
    cheight = std::max({target_height(), hmax[rows]+dpad, wheight});

    // calculate extra space that needs to be used.
    float wextra = std::max(0.0f, cwidth-wmax[columns]);
    float hextra = std::max(0.0f, cheight-hmax[rows]);

    cwpos = std::max(cwpos, wwidth-cwidth);
    chpos = std::max(chpos, wheight-cheight);

    float htotal = ualign+dalign, wtotal = lalign+ralign;
    for(float i : hfill) htotal += i;
    for(float i : wfill) wtotal += i;
    if(htotal == 0.0f) htotal = 1.0f;
    if(wtotal == 0.0f) wtotal = 1.0f;

    float hsum = ualign/htotal, wsum = lalign/wtotal;

    // use all space
    for(int32_t i=0; i<rows; i++){
        hmax[i] += hsum;
        hsum += hextra*(hfill[i]/htotal);
    } hmax[rows] += hsum;
    
    for(int32_t i=0; i<columns; i++){
        wmax[i] += wsum;
        wsum += wextra*(wfill[i]/wtotal);
    } wmax[columns] += wsum;

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            
            float wbegin = std::round(cwpos+wwpos+wmax[j]);
            float wend = std::round(cwpos+wwpos+wmax[std::min(columns, j+grid[i][j]->columnSpan)]);
            float hbegin = std::round(chpos+whpos+hmax[i]);
            float hend = std::round(chpos+whpos+hmax[std::min(rows, i+grid[i][j]->rowSpan)]);
            
            grid[i][j]->set_window_position(wbegin, hbegin);
            grid[i][j]->set_window_size(wend-wbegin, hend-hbegin);
            
            if(direction&1) grid[i][j]->update_grid(1);
        }
    }

    if(direction&2 && parent != this && parent != nullptr) parent->update_grid(2);

    refreshFlag = 1;
    
    return 0;
}

int32_t Frame::config_row(std::vector<float> hfill_){
   hfill = hfill_;
   hfill.resize(rows, 0);
   return 0;
}

int32_t Frame::config_column(std::vector<float> wfill_){
   wfill = wfill_;
   wfill.resize(columns, 0);
   return 0;
}

int32_t Frame::config_row(int32_t row, float value){
    hfill.resize(row+1, 0);
    hfill[row] = value;
    return 0;
}

int32_t Frame::config_column(int32_t column, float value){
    wfill.resize(column+1, 0);
    wfill[column] = value;
    return 0;
}

int32_t Frame::pad(float left, float right, float up, float down){
    lpad = left; rpad = right; upad = up; dpad = down;
    return 0;
}

int32_t Frame::align(float left, float right, float up, float down){
    lalign = left; ralign = right; ualign = up; dalign = down;
    return 0;
}

}

