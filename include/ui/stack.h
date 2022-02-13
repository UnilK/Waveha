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

    void insert(Frame *frame, unsigned index);
    void erase(unsigned index);
    void erase(Frame *frame);

    void clear();
    
    // find index of a frame. returns size() if frame is not found.
    unsigned find(Frame *frame);

    // access a frame in the stack
    Frame *get(unsigned index);
    Frame *get(Frame *frame);

    // resizing frames in the stack
    void push_resize(unsigned index, float size);
    void push_resize(Frame *frame, float size);
    void sticky_resize(unsigned index, float size);
    void sticky_resize(Frame *frame, float size);

    // find distance from frame beginning to end of visible stack.
    float max_size(unsigned index);
    float max_size(Frame *frame);

    // scroll the stack (not just the visible part) so that it begins at frame.
    void scroll_to(unsigned index);
    void scroll_to(Frame *frame);

    // swap the palce of frames in the stack
    void swap(unsigned a, unsigned b);
    void swap(Frame *a, Frame *b);

    // move frame up / down in the stack
    void move(unsigned index, int direction);
    void move(Frame *frame, int direction);

    // total sum of canvas sizes in the stack
    float length();

    // number of frames in the stack
    unsigned size();

    // use these for initialization
    void create(unsigned size);
    void put(Frame *frame, unsigned index);

    // update stack
    void update();

private:
    
    Side side;

    std::vector<Frame*> stack;

};

}
