#include "ui/frame.h"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <assert.h>

namespace ui{

Frame::Frame(Window *master_, kwargs values){ 
    master = master_;
    core = master->get_core();
    parent = nullptr;
    setup(values);
}

Frame::Frame(Frame *parent_, kwargs values){
    
    if(parent_ != nullptr){
        parent = parent_;
        core = parent->core;
        master = parent->master;
    }

    setup(values);
}

Frame::~Frame(){}

void Frame::set_parent(Frame *parent_){
    
    if(parent_ != nullptr){
        parent = parent_;
        core = parent->core;
        master = parent->master;
    } else {
        parent = nullptr;
    }

}

bool Frame::read_value(
        std::string key,
        std::stringstream &value,
        kwargs &values){
    if(values[key].empty()) return 0;
    value = std::stringstream(values[key]);
    return 1;
}

int32_t Frame::setup(kwargs &values){
    
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

std::string Frame::chars(std::string key){
    return core->style[look][key];
}

sf::Color Frame::color(std::string key){

    std::string color = core->style[look][key];
    
    if(color.size() == 6) color += "ff";
    while(color.size() < 8) color.push_back('0');
    
    uint32_t rgba;
    std::stringstream(color) >> std::hex >> rgba >> std::dec;

    return sf::Color(rgba>>24&0xff, rgba>>16&0xff, rgba>>8&0xff, rgba&0xff);
}

sf::Font &Frame::font(std::string key){
    return core->style.font(core->style[look][key]);
}

long double Frame::num(std::string key){
    long double x = 0;
    if(!core->style[look][key].empty()) std::stringstream(core->style[look][key]) >> x;
    return x;
}

uint32_t Frame::textStyle(std::string key){
    
    std::string styles = core->style[look][key], style;

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

std::vector<Frame*> Frame::find_focus(){
    std::vector<Frame*> focus;
    find_focus_inner(focus);
    return focus;
}

void Frame::find_focus_inner(std::vector<Frame*> &focus){

    focus.push_back(this);

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            Frame *child = grid[i][j];
            if(child->contains_mouse()){
                child->find_focus_inner(focus);
                break;
            }
        }
    }
}

Frame *Frame::get_parent(int32_t steps){
    if(steps < 1) return this;
    if(this->parent == nullptr) return nullptr;
    return parent->get_parent(steps - 1);
}

int32_t Frame::on_event(sf::Event event, int32_t priority){ return 0; }

int32_t Frame::core_tick(){
    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            grid[i][j]->core_tick();
        }
    }
    on_core_tick();
    return 0;
}

int32_t Frame::window_tick(){
    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            grid[i][j]->window_tick();
        }
    }
    on_window_tick();
    return 0;
}

int32_t Frame::on_core_tick(){ return 0; }

int32_t Frame::on_window_tick(){ return 0; }

int32_t Frame::on_reconfig(){ return 0; }

std::array<float, 2> Frame::global_mouse(){
    return {master->mouseX, master->mouseY};
}

std::array<float, 2> Frame::local_mouse(){
    return {
        master->mouseX - globalX + windowX + canvasX,
        master->mouseY - globalY + windowY + canvasY};
}

bool Frame::contains_mouse(){
    
    auto [x, y] = global_mouse();
    
    float left = globalX;
    float right = left + windowWidth;
    float up = globalY;
    float down = up + windowHeight;
    
    if(left <= x && x <= right && down >= y && y >= up) return 1;
    return 0;
}

bool Frame::degenerate(){
    return windowWidth < 0.5f || windowHeight < 0.5f;
}

int32_t Frame::draw(){
    return 0;
}

int32_t Frame::refresh(){

    if(reconfig_check()){
        on_reconfig();
        unset_reconfig();
    }

    if(refreshFlag){
        if(!degenerate()) draw();
        refreshFlag = 0;
    }

    if(degenerate()) return 0;
    
    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr){
                grid[i][j]->refresh();
            }
        }
    }

    return 0;
}

void Frame::set_refresh(){
    refreshFlag = 1;
    if(master != nullptr) master->set_refresh();
}

int32_t Frame::setup_grid(int32_t rows_, int32_t columns_){
    
    if(rows_ <= 0 || columns_ <= 0) return 1;
    
    rows = rows_;
    columns = columns_;
    
    grid = std::vector<std::vector<Frame*> >(rows, std::vector<Frame*>(columns, nullptr));
    heightFill = std::vector<float>(rows, 0);
    widthFill = std::vector<float>(columns, 0);
    heightMax = std::vector<float>(rows+1, 0);
    widthMax = std::vector<float>(columns+1, 0);
    update_grid();

    return 0;
}

int32_t Frame::resize_grid(int32_t rows_, int32_t columns_){
    
    if(rows_ <= 0 || columns_ <= 0) return 1;
    
    rows = rows_;
    columns = columns_;
    
    grid.resize(rows);
    for(auto &r : grid) r.resize(columns, nullptr);

    heightFill.resize(rows, 0);
    widthFill.resize(columns, 0);
    heightMax.resize(rows+1, 0);
    widthMax.resize(columns+1, 0);
    
    return 0;

}

int32_t Frame::update_grid(){

    set_refresh();
   
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
    
    if(rows) heightMax[rows] = std::max(heightMax[rows], heightMax[rows-1]);
    
    for(int32_t j=0; j<columns; j++){
        if(j) widthMax[j] = std::max(widthMax[j], widthMax[j-1]);
        for(int32_t i=0; i<rows; i++){
            if(grid[i][j] != nullptr){
                int32_t end = std::min(columns, j+grid[i][j]->columnSpan);
                widthMax[end] = std::max(widthMax[end], widthMax[j]+grid[i][j]->target_width());
            }
        }
    }
    
    if(columns) widthMax[columns] = std::max(widthMax[columns], widthMax[columns-1]);

    // calculate extra space that needs to be used.
    float widthExtra = std::max(0.0f, canvasWidth-widthMax[columns]);
    float heightExtra = std::max(0.0f, canvasHeight-heightMax[rows]);

    // move canvas displacement if necessary
    canvasX = std::max(0.0f, std::min(widthMax[columns] - windowWidth, canvasX));
    canvasY = std::max(0.0f, std::min(heightMax[rows] - windowHeight, canvasY));

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
            
            float childGlobalX = globalX + childWindowX - canvasX - windowX;

            float childGlobalY = globalY + childWindowY - canvasY - windowY;

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

int32_t Frame::place_frame(int32_t row, int32_t column, Frame *frame){
    int32_t ret = put(row, column, frame);
    update_grid();
    return ret;
}

int32_t Frame::put(int32_t row, int32_t column, Frame *frame){
    
    assert(row >= 0 && row < (int32_t)grid.size());
    assert(column >= 0 && column < (int32_t)grid[0].size());
    
    if(grid[row][column] == nullptr){
        frame->set_parent(this);
        grid[row][column] = frame;
        return 1;
    } else {
        grid[row][column]->set_parent(nullptr);
        frame->set_parent(this);
        grid[row][column] = frame;
        return 0;
    }
}

Frame *&Frame::get(int32_t row, int32_t column){

    assert(row >= 0 && row < (int32_t)grid.size());
    assert(column >= 0 && column < (int32_t)grid[0].size());

    return grid[row][column];
}

Frame *&Frame::get(Frame *frame){
    
    int32_t row = 0, column = 0;

    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == frame){
                row = i;
                column = j;
                break;
            }
        }
    }

    assert(row >= 0 && row < (int32_t)grid.size());
    assert(column >= 0 && column < (int32_t)grid[0].size());
    assert(grid[row][column] == frame);

    return grid[row][column];
}

int32_t Frame::remove_frame(int32_t row, int32_t column){
    
    assert(row >= 0 && row < (int32_t)grid.size());
    assert(column >= 0 && column < (int32_t)grid[0].size());

    grid[row][column]->set_parent(nullptr);
    grid[row][column] = nullptr;
    update_grid();

    return 0;
}

int32_t Frame::remove_frame(Frame *frame){
    
    for(int32_t i=0; i<rows; i++){
        for(int32_t j=0; j<columns; j++){
            if(grid[i][j] == frame){
                frame->set_parent(nullptr);
                grid[i][j] = nullptr;
                update_grid();
                return 0;
            }
        }
    }

    return 1;
}

int32_t Frame::set_window_size(float windowWidth_, float windowHeight_){

    float previousWidth = windowWidth;
    float previousHeight = windowHeight;

    windowWidth = std::max(0.0f, windowWidth_);
    windowHeight = std::max(0.0f, windowHeight_);
    
    if(windowWidth != previousWidth || windowHeight != previousHeight)
        windowResized = 1;

    return 0;
}

int32_t Frame::set_window_position(float windowX_, float windowY_){
    
    float previousX = windowX;
    float previousY = windowY;
    
    windowX = windowX_;
    windowY = windowY_;
    
    if(windowX != previousX || windowY != previousY)
        windowMoved = 1;
    
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

std::array<float, 2> Frame::get_target_canvas_size(){
    return {targetWidth, targetHeight};
}

int32_t Frame::set_canvas_size(float canvasWidth_, float canvasHeight_){
    
    float previousWidth = canvasWidth;
    float previousHeight = canvasHeight;

    canvasWidth = std::max(0.0f, canvasWidth_);
    canvasHeight = std::max(0.0f, canvasHeight_);
    
    if(canvasWidth != previousWidth || canvasHeight != previousHeight)
        canvasResized = 1;
    
    return 0;
}

int32_t Frame::set_canvas_position(float canvasX_, float canvasY_){
    
    float previousX = canvasX;
    float previousY = canvasY;
  
    canvasX = std::max(0.0f, std::min(widthMax[columns] - windowWidth, canvasX_));
    canvasY = std::max(0.0f, std::min(heightMax[rows] - windowHeight, canvasY_));

    if(canvasX != previousX || canvasY != previousY)
        canvasMoved = 1;
    
    return 0;
}

int32_t Frame::move_canvas(float deltaX, float deltaY){
    return set_canvas_position(canvasX + deltaX, canvasY + deltaY);
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

bool Frame::reconfig_check(){
    return windowMoved || windowResized || canvasMoved || canvasResized;
}

void Frame::unset_reconfig(){
    windowMoved = 0;
    windowResized = 0;
    canvasMoved = 0;
    canvasResized = 0;
}

SolidFrame::SolidFrame(Window *master_, kwargs values) :
    Frame(master_, values)
{
    background.setOutlineColor(color("borderColor"));
    background.setOutlineThickness(-num("borderThickness"));
    background.setFillColor(color("background"));
    on_reconfig();
}

SolidFrame::SolidFrame(Frame *parent_, kwargs values) :
    Frame(parent_, values)
{
    background.setOutlineColor(color("borderColor"));
    background.setOutlineThickness(-num("borderThickness"));
    background.setFillColor(color("background"));
    on_reconfig();
}

int32_t SolidFrame::on_reconfig(){

    background.setSize({windowWidth, windowHeight});

    return 0;
}

int32_t SolidFrame::draw(){
    
    background.setPosition(globalX, globalY);
    
    master->draw(background);
    
    return 0;
}



ContentFrame::ContentFrame(Window *master_, kwargs values) :
    Frame(master_, values)
{
    on_reconfig();
}

ContentFrame::ContentFrame(Frame *parent_, kwargs values) :
    Frame(parent_, values)
{
    on_reconfig();
}


int32_t ContentFrame::on_reconfig(){

    if(canvasWidth < 0.5f || canvasHeight < 0.5f) canvas.create(1, 1);
    else canvas.create(canvasWidth, canvasHeight);

    return inner_reconfig();
}

int32_t ContentFrame::inner_reconfig(){ return 0; }

int32_t ContentFrame::display(){

    sf::IntRect area(windowX, canvasHeight - windowY, windowWidth, -windowHeight);
    sf::Sprite sprite(canvas.getTexture(), area);
    sprite.setPosition(globalX, globalY);
    master->draw(sprite);

    return 0;
}

}

