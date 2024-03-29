#pragma once

#include "ui/frame.h"

#include <SFML/Graphics/Text.hpp>

#include <map>
#include <deque>
#include <functional>

namespace ui {

class Terminal;

class Command {
    
public:

    Command(Terminal&, const std::vector<std::string>&);

    std::string pop();
    bool empty();
    
    Terminal &source;

private:

    std::vector<std::string> stack;

};

class Directory {

public:

    Directory();
    virtual ~Directory();

    // returns whether a function was found for the command
    bool execute(Command);

    // soft construction, use during runtime. Keys can not be overridden.
    bool add_directory(std::string key, Directory *directory);
    bool add_function(std::string key, std::function<void(Command)> function);
    bool erase_entry(std::string key);

    // hard methods for initialization. assert guarded.
    void put_directory(std::string key, Directory *directory);
    void put_function(std::string key, std::function<void(Command)> function);

    // add short documentations
    void document(std::string key, std::string doc);

    Directory *find(Command);

protected:

    const std::string HELP = "help", LIST = "ls";

private:
    
    void print_help(Command);
    void print_list(Command);
    
    std::map<std::string, Directory*> directories;
    std::map<std::string, std::function<void(Command)> > functions;
    std::map<std::string, std::string> docs;

};

class Terminal : public Frame, public Directory {

    /*
       style:
       
       background (color)
       borderColor (color)
       borderThickness (num) or (num(left) num(right) num(up) num(down))
       
       textSize (num)
       font (font)
       
       commandColor (color)
       outputColor (color)
       errorColor (color)

       Note:
       The syntax used in commands is similar to shell, but
       spaces are used instead of forward slashes
       in directory navigation: ./bin/kick -o can -> . bin kick -o can
       the forward slash often has an unergonomic
       key position on non-us keyboard layouts.
     */

public:

    Terminal(Window *master_, Kwargs = {});
    virtual ~Terminal();

    Capture on_event(sf::Event event, int priority);

    void set_look(std::string look_);
    void on_refresh();

    // add text to be displayed on the frame.
    void push_output(std::string);
    void push_error(std::string);
    void push_command(std::string);

    void cd(Command);
    void pwd(Command);
    void clear(Command);

protected:

    struct Line { std::string text, color; };

private:
    
    std::vector<std::string> create_command(std::string);
    std::vector<std::string> wrap(std::string, unsigned);

    // editing
    std::deque<std::string> edits, originals;
    std::set<unsigned> edited;
    std::string prefix = "$ ";
    unsigned editX = 0, editY = 0;
    sf::Text blinker;

    // command history
    std::deque<Line> buffer;
    unsigned bufferMax = 1000;
    unsigned bufferOffset = 0;

    void clean_buffer();

    // use "." as a short hand for the address:
    // address = {"bin", "glass"}
    // . recycle "wine bottle" -> {bin,glass,recycle,wine bottle}
    // .recycle "wine bottle" -> {.recycle,wine bottle} (error).
    std::vector<std::string> address, previous;
    
    // text and it's placement on the screen
    sf::Text text;
    float offsetX = 0, offsetY = 0;

};

}

