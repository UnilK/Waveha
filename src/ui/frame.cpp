#include "ui/frame.h"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>

namespace ui{

Frame::Frame(Window *master_, std::map<std::string, std::string> values){ 
    master = master_;
    core = master->get_core();
    parent = nullptr;
    setup(values);
}

Frame::Frame(Frame *parent_, std::map<std::string, std::string> values){
    parent = parent_;
    core = parent->core;
    master = parent->master;
    setup(values);
}

bool Frame::read_value(
        std::string key,
        std::stringstream &value,
        std::map<std::string, std::string> &values){
    if(values[key].empty()) return 0;
    value = std::stringstream(values[key]);
    return 1;
}

int32_t Frame::setup(std::map<std::string, std::string> &values){
    
    std::stringstream value;

    if(read_value("look", value, values))
        value >> look;
    
    if(read_value("id", value, values))
        value >> id;
    
    if(read_value("width", value, values))
        value >> targetWidth;
    
    if(read_value("height", value, values))
        value >> targetHeight;
    
    if(read_value("columnSpan", value, values))
        value >> columnSpan;
    
    if(read_value("rowSpan", value, values))
        value >> rowSpan;
    
    if(read_value("pad", value, values))
        value >> alignPadLeft >> alignPadRight >> alignPadUp >> alignPadDown;
    
    if(read_value("fill", value, values)){
        value >> alignFillLeft >> frameFillWidth >> alignFillRight
            >> alignFillUp >> frameFillHeight >> alignFillDown;
    }

    return 0;
}
        
int32_t Frame::event_update(sf::Event){ return 0; }

int32_t Frame::coreapp_update(){ return 0; }

std::array<float, 2> Frame::global_mouse(){
    return {master->mouseX, master->mouseY};
}

std::array<float, 2> Frame::local_mouse(){
    return {
        master->mouseX - globalX + windowX,
        master->mouseY - globalY + windowY};
}

Frame *Frame::find_focus(){

    auto [x, y] = global_mouse();

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            
            Frame *child = grid[i][j];
            
            float left = child->globalX;
            float right = left + child->windowWidth;
            float up = child->globalY;
            float down = up + child->windowHeight;
            
            if(left <= x && x <= right && down >= y && y >= up){
                return grid[i][j]->find_focus(); 
            }
        }
    }

    return this;
}

bool Frame::degenerate(){
    return windowWidth < 0.5f || windowHeight < 0.5f;
}

int32_t Frame::draw(){
    return 0;
}

int32_t Frame::refresh(){

    if(degenerate()) return 0;

    if(refreshFlag){
        draw();
        refreshFlag = 0;
        master->displayFlag = 1;
    }
    
    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr) grid[i][j]->refresh();
        }
    }

    return 0;
}

int32_t Frame::setup_grid(int32_t rows_, int32_t columns_){
    
    if(rows_ <= 0 || columns_ <= 0) return 1;
    
    rows = rows_;
    columns = columns_;
    
    grid = std::vector<std::vector<Frame*> >(rows, std::vector<Frame*>(columns, nullptr));
    heightFill.resize(rows, 0);
    widthFill.resize(columns, 0);
    heightMax.resize(rows+1, 0);
    widthMax.resize(columns+1, 0);
    
    return 0;
}

int32_t Frame::update_grid(){

    refreshFlag = 1;
   
    if(rows == 0 || columns == 0) return 0;

    std::fill(heightMax.begin(), heightMax.end(), 0);
    std::fill(widthMax.begin(), widthMax.end(), 0);
   
    // calculate grid dimensions. Each row & column size is determined by the most
    // space consuming frame that ends there.
    for(int32_t i=0; i<rows; i++){
        if(i) heightMax[i] = std::max(heightMax[i], heightMax[i-1]);
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr){
                int32_t end = std::min(rows, i+grid[i][j]->rowSpan);
                heightMax[end] = std::max(heightMax[end], heightMax[i]+grid[i][j]->target_height());
            }
        }
    }
    
    for(int32_t j=0; j<columns; j++){
        if(j) widthMax[j] = std::max(widthMax[j], widthMax[j-1]);
        for(int32_t i=0; i<rows; i++){
            if(grid[i][j] != nullptr){
                int32_t end = std::min(columns, j+grid[i][j]->columnSpan);
                widthMax[end] = std::max(widthMax[end], widthMax[j]+grid[i][j]->target_width());
            }
        }
    }

    // calculate extra space that needs to be used.
    float widthExtra = std::max(0.0f, canvasWidth-widthMax[columns]);
    float heightExtra = std::max(0.0f, canvasHeight-heightMax[rows]);

    // move canvas displacement if necessary
    canvasX = std::min(canvasX, widthExtra);
    canvasY = std::min(canvasY, heightExtra);

    float heightTotal = 0, widthTotal = 0;
    for(float i : heightFill) heightTotal += i;
    for(float i : widthFill) widthTotal += i;
    if(heightTotal == 0.0f) heightTotal = 1.0f;
    if(widthTotal == 0.0f) widthTotal = 1.0f;

    float heightSum = 0, widthSum = 0;

    // allocate extra space
    for(int32_t i=0; i<rows; i++){
        heightMax[i] += heightSum;
        heightSum += heightExtra*(heightFill[i]/heightTotal);
    } heightMax[rows] += heightSum;
    
    for(int32_t i=0; i<columns; i++){
        widthMax[i] += widthSum;
        widthSum += widthExtra*(widthFill[i]/widthTotal);
    } widthMax[columns] += widthSum;

    // do the windowing
    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            
            Frame *child = grid[i][j];

            float gridLeft = widthMax[j];
            float gridUp = heightMax[i];
            float gridRight = widthMax[std::min(columns, j+child->columnSpan)];
            float gridDown = heightMax[std::min(rows, i+child->rowSpan)];
            
            float extraX = std::round(gridRight - gridLeft) - child->target_width();
            float extraY = std::round(gridDown - gridUp) - child->target_height();

            float fillX = child->alignFillLeft + child->frameFillWidth + child->alignFillRight;
            float fillY = child->alignFillUp + child->frameFillHeight + child->alignFillDown;

            if(fillX == 0.0f) fillX = 1;
            if(fillY == 0.0f) fillY = 1;

            float childCanvasX = std::round(
                    gridLeft + child->alignPadLeft
                    + extraX * child->alignFillLeft / fillX);

            float childCanvasY = std::round(
                    gridUp + child->alignPadUp
                    + extraY * child->alignFillUp / fillY);
            
            float childCanvasWidth = std::round(
                    child->targetWidth + extraX * child->frameFillWidth / fillX);

            float childCanvasHeight = std::round(
                    child->targetHeight + extraY * child->frameFillHeight / fillY);
            
            float childWindowX = std::round(
                    std::max(childCanvasX, canvasX + windowX));

            float childWindowY = std::round(
                    std::max(childCanvasY, canvasY + windowY));
            
            float childGlobalX = globalX + childWindowX - canvasX;

            float childGlobalY = globalY + childWindowY - canvasY;

            float childWindowWidth = std::round(
                    std::min(childCanvasX + childCanvasWidth, canvasX + windowX + windowWidth)
                    - childWindowX);

            float childWindowHeight = std::round(
                    std::min(childCanvasY + childCanvasHeight, canvasY + windowY + windowHeight)
                    - childWindowY);
           
            child->set_global_position(childGlobalX, childGlobalY);
            
            child->set_window_position(childWindowX - childCanvasX, childWindowY - childCanvasY);
           
            child->set_canvas_size(childCanvasWidth, childCanvasHeight);

            child->set_window_size(childWindowWidth, childWindowHeight);

            child->update_grid();
        }
    }
    
    return 0;
}

int32_t Frame::set_window_size(float windowWidth_, float windowHeight_){
    windowWidth = std::max(0.0f, windowWidth_);
    windowHeight = std::max(0.0f, windowHeight_);
    return 0;
}

int32_t Frame::set_window_position(float windowX_, float windowY_){
    windowX = windowX_;
    windowY = windowY_;
    return 0;
}

int32_t Frame::set_global_position(float globalX_, float globalY_){
    globalX = globalX_;
    globalY = globalY_;
    return 0;
}

int32_t Frame::set_target_size(float targetWidth_, float targetHeight_){
    targetWidth = std::max(0.0f, targetWidth_);
    targetHeight = std::max(0.0f, targetHeight_);
    return 0;
}

int32_t Frame::set_canvas_size(float canvasWidth_, float canvasHeight_){
    canvasWidth = std::max(0.0f, canvasWidth_);
    canvasHeight = std::max(0.0f, canvasHeight_);
    return 0;
}

int32_t Frame::set_canvas_position(float canvasX_, float canvasY_){
    canvasX = canvasX_;
    canvasY = canvasY_;
    return 0;
}

int32_t Frame::move_canvas(float deltaX, float deltaY){
    canvasX += deltaX;
    canvasY += deltaY;
    return 0;
}

float Frame::target_width(){
    return targetWidth + alignPadLeft + alignPadRight;
}

float Frame::target_height(){
    return targetHeight + alignPadUp + alignPadDown;
}


int32_t Frame::fill_height(std::vector<float> heightFill_){
    heightFill = heightFill_;
    heightFill.resize(rows, 0);
    return 0;
}

int32_t Frame::fill_width(std::vector<float> widthFill_){
    widthFill = widthFill_;
    widthFill.resize(columns, 0);
    return 0;
}

int32_t Frame::fill_height(int32_t row, float value){
   heightFill[row] = value;
   return 0;
}

int32_t Frame::fill_width(int32_t column, float value){
    widthFill[column] = value;
    return 0;
}

int32_t Frame::align(float left, float right, float up, float down){
    alignPadLeft = left;
    alignPadRight = right;
    alignPadDown = up;
    alignPadUp = down;
    return 0;
}

int32_t Frame::align_fill(float left, float right, float up, float down){
    alignFillLeft = left;
    alignFillRight = right;
    alignFillDown = up;
    alignFillUp = down;
    return 0;
}

int32_t Frame::frame_fill(float width, float height){
    frameFillWidth = width;
    frameFillHeight = height;
    return 0;
}

int32_t Frame::set_span(int32_t rowSpan_, int32_t columnSpan_){
    rowSpan = std::max(1, rowSpan_);
    columnSpan = std::max(1, columnSpan_);
    return 0;
};



SolidFrame::SolidFrame(Window *master_, std::map<std::string, std::string> values) :
    Frame(master_, values) {}

SolidFrame::SolidFrame(Frame *parent_, std::map<std::string, std::string> values) :
    Frame(parent_, values) {}


int32_t SolidFrame::draw(){
    
    background.setSize({windowWidth, windowHeight});
    background.setPosition(globalX, globalY);
    background.setFillColor(core->style.get(look).color("background"));
    master->draw(background);
    
    return 0;
}



ContentFrame::ContentFrame(Window *master_, std::map<std::string, std::string> values) :
    Frame(master_, values) {}

ContentFrame::ContentFrame(Frame *parent_, std::map<std::string, std::string> values) :
    Frame(parent_, values) {}


int32_t ContentFrame::initialize(){

    if(canvasWidth < 0.5f || canvasHeight < 0.5f) canvas.create(1, 1);
    else canvas.create(canvasWidth, canvasHeight);

    return 0;
}

int32_t ContentFrame::display(){

    sf::IntRect area(windowX, windowY, windowWidth, windowHeight);
    sf::Sprite sprite(canvas.getTexture(), area);
    sprite.setPosition(globalX, globalY);
    master->draw(sprite);

    return 0;
}

}

