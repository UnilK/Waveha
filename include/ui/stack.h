#pragma once

#include "ui/frame.h"

#include <vector>

namespace ui {

enum Side { left, right, up, down };

class Stack : public Frame {

    // class from managing a stack of frames.

public:

    // side points "the direction of gravity" for the stack
    Stack(Window *master_, Side, Kwargs = {});

    // adding / removing stuff from the stack
    void push_back(Frame *frame);
    void pop_back();

    void insert(Frame *frame, uint32_t index);
    void erase(uint32_t index);
    void erase(Frame *frame);

    void clear();

    // resizing frames in the stack
    void push_resize(uint32_t index, float size);
    void push_resize(Frame *frame, float size);
    void sticky_resize(uint32_t index, float size);
    void sticky_resize(Frame *frame, float size);

    // find distance from frame beginning to end of visible stack.
    float max_size(uint32_t index);
    float max_size(Frame *frame);

    // scroll the stack (not just the visible part) so that it begins at frame.
    void scroll_to(uint32_t index);
    void scroll_to(Frame *frame);

    // swap the palce of frames in the stack
    void swap(uint32_t a, uint32_t b);
    void swap(Frame *a, Frame *b);

    // move frame up / down in the stack
    void move(uint32_t index, int32_t direction);
    void move(Frame *frame, int32_t direction);

    // total sum of canvas sizes in the stack
    float length();

    // number of frames in the stack
    uint32_t size();

    // use these for initialization
    void create(uint32_t size);
    void put(Frame *frame, uint32_t index);
    void seal();

private:
    
    void update();

    Side side;
    Frame dummy;

    std::vector<Frame*> stack;

};

}
