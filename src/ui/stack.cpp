#include "ui/stack.h"

#include <algorithm>
#include <assert.h> 
#include <iostream>

namespace ui {

Stack::Stack(Window *master_, Side side_, Kwargs kwargs) :
    Frame(master_, kwargs),
    side(side_)
{ 
    stack.push_back(nullptr);

    setup_grid(1, 1);

    fill_width(0, 1);
    fill_height(0, 1);

    auto_contain(0);
}

void Stack::push_back(Frame *frame){
    stack.push_back(frame);
    std::swap(stack[size()], stack[size() - 1]);
    update();
}

void Stack::pop_back(){
    if(size() == 0) return;
    std::swap(stack[size()], stack[size() - 1]);
    stack.pop_back();
    update();
}

void Stack::insert(Frame *frame, uint32_t index){
    index = std::min(index, size());
    stack.push_back(frame);
    std::swap(stack[size()], stack[size() - 1]);
    move(size() - 1, (int32_t)index + 1 - size());
}

void Stack::erase(uint32_t index){
    assert(index < size());
    move(index, size() - 1 - index);
    pop_back();
}

void Stack::erase(Frame *frame){
    erase(find(frame));
}

void Stack::clear(){
   
    stack.clear();

    stack.push_back(nullptr);

    setup_grid(1, 1);

    fill_width(0, 1);
    fill_height(0, 1);

}

uint32_t Stack::find(Frame *frame){
    uint32_t index = 0;
    while(index < size() && stack[index] != frame) index++;
    return index;
}

Frame *Stack::get(uint32_t index){
    assert(index < size());
    return stack[index];
}

Frame *Stack::get(Frame *frame){
    return get(find(frame));
}

void Stack::push_resize(uint32_t index, float csize){
    
    assert(index < size());

    Frame *frame = stack[index];

    if(side == Side::left || side == Side::right){
        frame->set_target_size(csize, frame->targetHeight);
    } else {
        frame->set_target_size(frame->targetWidth, csize);
    }

    update_grid();
}

void Stack::push_resize(Frame *frame, float csize){
    push_resize(find(frame), csize);
}

void Stack::sticky_resize(uint32_t index, float csize){
    
    assert(index < size());

    Frame *frame = stack[index];

    if(side == Side::left || side == Side::right){
        
        float begin = frame->targetWidth;
        frame->set_target_size(csize, frame->targetHeight);
        float end = frame->targetWidth;

        float delta, total = begin - end;

        index++;

        while(index < size() && std::abs(total) > 0.01f){
            
            frame = stack[index];
            begin = frame->targetWidth;
            frame->set_target_size(frame->targetWidth + total, frame->targetHeight);
            end = frame->targetWidth;

            delta = end - begin;
            total -= delta;
    
            index++;
        }

    } else {
        
        float begin = frame->targetHeight;
        frame->set_target_size(frame->targetWidth, csize);
        float end = frame->targetHeight;

        float delta, total = begin - end;

        index++;

        while(index < size() && std::abs(total) > 0.01f){
            
            frame = stack[index];
            begin = frame->targetHeight;
            frame->set_target_size(frame->targetWidth, frame->targetHeight + total);
            end = frame->targetHeight;

            delta = end - begin;
            total -= delta;
    
            index++;
        }
    }

    update_grid();
}

void Stack::sticky_resize(Frame *frame, float csize){
    sticky_resize(find(frame), csize);
}

float Stack::max_size(uint32_t index){
    
    assert(index < size());

    Frame *frame = stack[index];

    if(side == Side::left || side == Side::right){
        return windowWidth - frame->globalX + globalX;
    } else {
        return windowHeight - frame->globalY - globalY;
    }
}

float Stack::max_size(Frame *frame){
    return max_size(find(frame));
}

void Stack::scroll_to(uint32_t index){
    
    assert(index < size());

    Frame *frame = stack[index];

    float deltaX = 0, deltaY = 0; 

    if(side == Side::left || side == Side::right){
        deltaX = frame->globalX - frame->windowX - globalX;
    } else {
        deltaY = frame->globalY - frame->windowY - globalY;
    }
   
    move_canvas(deltaX, deltaY);
    update_grid();
}

void Stack::scroll_to(Frame *frame){
    scroll_to(find(frame));
}

void Stack::swap(uint32_t a, uint32_t b){
    
    assert(a < size() && b < size());

    std::swap(stack[a], stack[b]);
    
    update();
}

void Stack::swap(Frame *a, Frame *b){
    swap(find(a), find(b));
}

void Stack::move(uint32_t index, int32_t direction){
    
    assert(index < size());
    
    uint32_t left = index, right = 0;
    if(direction < 0 && (uint32_t)(-direction) > index) right = 0;
    else right = std::min(size() - 1, index + direction);

    if(left > right) std::swap(left, right);

    if(direction < 0){
        while(left < right){
            std::swap(stack[right - 1], stack[right]);
            right--;
        }
    }
    else {
        while(left < right){
            std::swap(stack[left + 1], stack[left]);
            left++;
        }
    }

    update();
}

void Stack::move(Frame *frame, int32_t direction){
    move(find(frame), direction);
}

float Stack::length(){
    
    float total = 0;
    
    if(side == Side::left || side == Side::right){
        for(Frame *frame : stack) total += frame->target_width();
    } else {
        for(Frame *frame : stack) total += frame->target_height();
    }

    return total;
}

uint32_t Stack::size(){
    return stack.size() - 1;
}

void Stack::create(uint32_t size){
    stack = std::vector<Frame*>(size + 1, nullptr);
}

void Stack::put(Frame *frame, uint32_t index){
    assert(index < size());
    stack[index] = frame;
}

void Stack::update(){
    
    switch(side){
        case Side::left:

            setup_grid(1, stack.size());
            for(uint32_t i=0; i<stack.size(); i++) Frame::put(0, i, stack[i]);
            fill_width(size(), 1);
            fill_height(0, 1);

            break;
        case Side::right:

            setup_grid(1, stack.size());
            for(uint32_t i=0; i<stack.size(); i++) Frame::put(0, size() - i, stack[i]);
            fill_width(0, 1);
            fill_height(0, 1);

            break;
        case Side::up:
            
            setup_grid(stack.size(), 1);
            for(uint32_t i=0; i<stack.size(); i++) Frame::put(i, 0, stack[i]);
            fill_width(0, 1);
            fill_height(size(), 1);
            
            break;
        case Side::down:
            
            setup_grid(stack.size(), 1);
            for(uint32_t i=0; i<stack.size(); i++) Frame::put(size() - i, 0, stack[i]);
            fill_width(0, 1);
            fill_height(0, 1);
            
            break;
    }
    
    update_grid();
}

}
