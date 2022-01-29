#include "ui/frame.h"
#include "ui/window.h"

#include <SFML/Graphics/Text.hpp>

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <math.h>
#include <assert.h>

namespace ui {

Frame::Frame(Window *master_, Kwargs kwargs){ 
    
    master = master_;
    parent = nullptr;

    look = kwargs.look;

    targetWidth = kwargs.width;
    targetHeight = kwargs.height;
    widthMin = kwargs.range[0];
    widthMax = kwargs.range[1];
    heightMin = kwargs.range[2];
    heightMax = kwargs.range[3];
    
    columnSpan = std::max(1u, kwargs.spanx);
    rowSpan = std::max(1u, kwargs.spany);
    
    alignPadLeft = kwargs.pad[0];
    alignPadRight = kwargs.pad[1];
    alignPadUp = kwargs.pad[2];
    alignPadDown = kwargs.pad[3]; 

    alignFillLeft = kwargs.fill[0];
    frameFillWidth = kwargs.fill[1]; 
    alignFillRight = kwargs.fill[2];
    alignFillUp = kwargs.fill[3];
    frameFillHeight = kwargs.fill[4];
    alignFillDown = kwargs.fill[5];

    auto [bl, br, bu, bd] = kwargs.border;
    set_border(bl, br, bu, bd);
    
    set_look(look);
}

Frame::~Frame(){
    if(master->get_hard_focus() == this) master->reset_hard_focus();
}

// events and actions /////////////////////////////////////////////////////////

void Frame::set_look(std::string look_){
    look = look_;
    border.set_look(look);
}

Frame::Capture Frame::on_event(sf::Event event, int32_t priority){ return Capture::pass; }

void Frame::tick(){
    
    on_tick();
    
    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            grid[i][j]->tick();
        }
    }
}

void Frame::on_tick(){}

void Frame::on_reconfig(){
    border.set_size(canvasWidth, canvasHeight);
}

void Frame::set_reconfig(){
    reconfigFlag = 1;
    set_refresh();
}

bool Frame::degenerate(){
    return windowWidth < 0.5f || windowHeight < 0.5f;
}

void Frame::refresh(){

    if(reconfigFlag){
        on_reconfig();
        reconfigFlag = 0;
    }
    
    auto [masterWidth, masterHeight] = master->getSize();

    if(degenerate()) return;

    if(refreshFlag){
        
        assert(master != nullptr);

        // set drawing area to a subrectangle on the window.
        
        sf::View w(sf::FloatRect(windowX, windowY, windowWidth, windowHeight));
        
        w.setViewport(sf::FloatRect(
                    globalX / masterWidth,
                    globalY / masterHeight,
                    windowWidth / masterWidth,
                    windowHeight / masterHeight));
        
        master->setView(w);

        on_refresh();
    }
    
    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr){
                if(refreshFlag) grid[i][j]->set_refresh(); 
                grid[i][j]->refresh();
            }
        }
    }
    
    refreshFlag = 0;
}

void Frame::on_refresh(){
    border.draw(*master);
}

void Frame::set_refresh(){
    refreshFlag = 1;
    if(master != this) master->set_refresh();
}

std::array<float, 2> Frame::global_mouse(){
    return master->mouse();
}

std::array<float, 2> Frame::local_mouse(){
    auto [x, y] = master->mouse();
    return {
        x - globalX + windowX + canvasX,
        y - globalY + windowY + canvasY};
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

std::vector<Frame*> Frame::find_focus(){
    std::vector<Frame*> focus;
    find_focus_inner(focus);
    return focus;
}

void Frame::find_focus_inner(std::vector<Frame*> &focus){

    focus.push_back(this);

    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            Frame *child = grid[i][j];
            if(child->contains_mouse()){
                child->find_focus_inner(focus);
                break;
            }
        }
    }
}

// window management //////////////////////////////////////////////////////////

void Frame::set_window_size(float windowWidth_, float windowHeight_){

    float previousWidth = windowWidth;
    float previousHeight = windowHeight;

    windowWidth = std::max(0.0f, windowWidth_);
    windowHeight = std::max(0.0f, windowHeight_);
    
    if(windowWidth != previousWidth || windowHeight != previousHeight)
        set_reconfig();
}

void Frame::set_window_position(float windowX_, float windowY_){
    
    float previousX = windowX;
    float previousY = windowY;
    
    windowX = windowX_;
    windowY = windowY_;
    
    if(windowX != previousX || windowY != previousY)
        set_reconfig();
}

void Frame::set_global_position(float globalX_, float globalY_){
    
    globalX = globalX_;
    globalY = globalY_;
}

void Frame::set_canvas_size(float canvasWidth_, float canvasHeight_){
    
    float previousWidth = canvasWidth;
    float previousHeight = canvasHeight;

    canvasWidth = std::max(0.0f, canvasWidth_);
    canvasHeight = std::max(0.0f, canvasHeight_);
    
    if(canvasWidth != previousWidth || canvasHeight != previousHeight)
        set_reconfig();
}

void Frame::set_canvas_position(float canvasX_, float canvasY_){
    
    float previousX = canvasX;
    float previousY = canvasY;
 
    canvasX = canvasX_;
    canvasY = canvasY_;

    if(canvasX != previousX || canvasY != previousY)
        set_reconfig();
}

void Frame::move_canvas(float deltaX, float deltaY){
    set_canvas_position(canvasX + deltaX, canvasY + deltaY);
}

void Frame::set_target_size(float targetWidth_, float targetHeight_){
    targetWidth = std::max(widthMin, std::min(widthMax, targetWidth_));
    targetHeight = std::max(heightMin, std::min(heightMax, targetHeight_));
}

void Frame::fit_content(){
    set_target_size(contentWidth, contentHeight);
}

void Frame::set_range(float wMin, float wMax, float hMin, float hMax){
    assert(wMin < wMax && hMin < hMax && wMin >= 0 && hMin >= 0);
    widthMin = wMin;
    widthMax = wMax;
    heightMin = hMin;
    heightMax = hMax;
}

float Frame::target_width(){
    return targetWidth + alignPadLeft + alignPadRight;
}

float Frame::target_height(){
    return targetHeight + alignPadUp + alignPadDown;
}

void Frame::auto_contain(bool autoContain_){ autoContain = autoContain_; }

// grid management ////////////////////////////////////////////////////////////

void Frame::setup_grid(uint32_t rows_, uint32_t columns_){
    
    rows = rows_;
    columns = columns_;
    
    grid = std::vector<std::vector<Frame*> >(rows, std::vector<Frame*>(columns, nullptr));
    heightFill = std::vector<float>(rows, 0);
    widthFill = std::vector<float>(columns, 0);
}

void Frame::resize_grid(uint32_t rows_, uint32_t columns_){
    
    rows = rows_;
    columns = columns_;
    
    grid.resize(rows);
    for(auto &r : grid) r.resize(columns, nullptr);

    heightFill.resize(rows, 0);
    widthFill.resize(columns, 0);
}

void Frame::update_grid(){

    set_refresh();
   
    if(rows == 0 || columns == 0) return;

    std::vector<float> widthGrid(columns + 1, 0), heightGrid(rows + 1, 0);
   
    // calculate grid dimensions. Each row & column size is determined by the most
    // space consuming frame that ends there.
    for(uint32_t i=0; i<rows; i++){
        if(i > 0) heightGrid[i] = std::max(heightGrid[i], heightGrid[i-1]);
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr){
                uint32_t end = std::min(rows, i+grid[i][j]->rowSpan);
                heightGrid[end] = std::max(heightGrid[end], heightGrid[i]+grid[i][j]->target_height());
            }
        }
    }
    
    if(rows > 0) heightGrid[rows] = std::max(heightGrid[rows], heightGrid[rows-1]);
    
    for(uint32_t j=0; j<columns; j++){
        if(j > 0) widthGrid[j] = std::max(widthGrid[j], widthGrid[j-1]);
        for(uint32_t i=0; i<rows; i++){
            if(grid[i][j] != nullptr){
                uint32_t end = std::min(columns, j+grid[i][j]->columnSpan);
                widthGrid[end] = std::max(widthGrid[end], widthGrid[j]+grid[i][j]->target_width());
            }
        }
    }
    
    if(columns > 0) widthGrid[columns] = std::max(widthGrid[columns], widthGrid[columns-1]);
    
    contentWidth = widthGrid[columns];
    contentHeight = heightGrid[rows];

    // calculate extra space that needs to be used.
    float widthExtra = std::max(0.0f, canvasWidth-widthGrid[columns]);
    float heightExtra = std::max(0.0f, canvasHeight-heightGrid[rows]);

    // move canvas displacement if necessary
    if(autoContain){
        canvasX = std::max(0.0f, std::min(widthGrid[columns] - windowWidth, canvasX));
        canvasY = std::max(0.0f, std::min(heightGrid[rows] - windowHeight, canvasY));
    }

    float heightTotal = 0, widthTotal = 0;
    for(float i : heightFill) heightTotal += i;
    for(float i : widthFill) widthTotal += i;
    if(heightTotal == 0.0f) heightTotal = 1.0f;
    if(widthTotal == 0.0f) widthTotal = 1.0f;

    float heightSum = 0, widthSum = 0;

    // allocate extra space
    for(uint32_t i=0; i<rows; i++){
        heightGrid[i] += heightSum;
        heightSum += heightExtra*(heightFill[i]/heightTotal);
    } heightGrid[rows] += heightSum;
    
    for(uint32_t i=0; i<columns; i++){
        widthGrid[i] += widthSum;
        widthSum += widthExtra*(widthFill[i]/widthTotal);
    } widthGrid[columns] += widthSum;
  
    // do the windowing
    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] == nullptr) continue;
            
            Frame *child = grid[i][j];

            float gridLeft = widthGrid[j];
            float gridUp = heightGrid[i];
            float gridRight = widthGrid[std::min(columns, j+child->columnSpan)];
            float gridDown = heightGrid[std::min(rows, i+child->rowSpan)];
            
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
}

void Frame::set_parent(Frame *parent_){
    
    if(parent_ != nullptr){
        parent = parent_;
        master = parent->master;
    } else {
        parent = nullptr;
    }
}

Frame *Frame::get_parent(int32_t steps){
    if(steps < 1) return this;
    if(parent == nullptr) return nullptr;
    return parent->get_parent(steps - 1);
}

Frame *Frame::get_top(){
    if(parent == nullptr || parent == this) return this;
    return parent->get_top();
}

Window *Frame::get_master(){ return master; }

int32_t Frame::place_frame(uint32_t row, uint32_t column, Frame *frame){
    int32_t ret = put(row, column, frame);
    update_grid();
    return ret;
}

int32_t Frame::remove_frame(uint32_t row, uint32_t column){
    
    assert(row < rows && column < columns);

    grid[row][column]->set_parent(nullptr);
    grid[row][column] = nullptr;
    update_grid();

    return 0;
}

int32_t Frame::remove_frame(Frame *frame){
    
    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
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

int32_t Frame::put(uint32_t row, uint32_t column, Frame *frame){
    
    assert(row < rows && column < columns);
    
    if(grid[row][column] == nullptr){
        if(frame != nullptr) frame->set_parent(this);
        grid[row][column] = frame;
        return 1;
    } else {
        grid[row][column]->set_parent(nullptr);
        if(frame != nullptr) frame->set_parent(this);
        grid[row][column] = frame;
        return 0;
    }
}

Frame *&Frame::get(uint32_t row, uint32_t column){

    assert(row < rows && column < columns);

    return grid[row][column];
}

Frame *&Frame::get(Frame *frame){
    
    uint32_t row = 0, column = 0;

    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] == frame){
                row = i;
                column = j;
                break;
            }
        }
    }

    assert(grid[row][column] == frame);

    return grid[row][column];
}

void Frame::fill_height(std::vector<float> heightFill_){
    assert(heightFill_.size() == rows);
    heightFill = heightFill_;
}

void Frame::fill_width(std::vector<float> widthFill_){
    assert(widthFill_.size() == columns);
    widthFill = widthFill_;
}

void Frame::fill_height(uint32_t row, float value){
    assert(row < rows);
    heightFill[row] = value;
}

void Frame::fill_width(uint32_t column, float value){
    assert(column < columns);
    widthFill[column] = value;
}

void Frame::set_span(uint32_t rowSpan_, uint32_t columnSpan_){
    rowSpan = std::max(1u, rowSpan_);
    columnSpan = std::max(1u, columnSpan_);
};


// cosmetics //////////////////////////////////////////////////////////////////

void Frame::pad(float left, float right, float up, float down){
    alignPadLeft = left;
    alignPadRight = right;
    alignPadDown = up;
    alignPadUp = down;
}

void Frame::fill(float left, float midh, float right, float up, float midv, float down){
    alignFillLeft = left;
    frameFillWidth = midh;
    alignFillRight = right;
    alignFillDown = up;
    frameFillHeight = midv;
    alignFillUp = down;
}

void Frame::update_style(){
    
    set_look(look);

    for(uint32_t i=0; i<rows; i++){
        for(uint32_t j=0; j<columns; j++){
            if(grid[i][j] != nullptr){
                grid[i][j]->update_style();
            }
        }
    }

    if(this == master) update_grid();
}

void Frame::set_border(bool left, bool right, bool up, bool down){
    border.set_border(left, right, up, down);
    border.update();
}


}
