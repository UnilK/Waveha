#include "ui/terminal.h"

#include <algorithm>
#include <math.h>
#include <assert.h>

namespace ui {

// command ////////////////////////////////////////////////////////////////////

Command::Command(Terminal &src, const std::vector<std::string> &stk) : 
    source(src),
    stack(stk)
{
    std::reverse(stack.begin(), stack.end());
}

std::string Command::pop(){
    
    std::string front = "";
    
    if(!stack.empty()){
        front = stack.back();
        stack.pop_back();
    }

    return front;
}

bool Command::empty(){ return stack.empty(); }

// directory //////////////////////////////////////////////////////////////////

Directory::Directory(){
    put_function(HELP, [&](Command c){ print_help(c); });
    put_function(LIST, [&](Command c){ print_list(c); });
}

Directory::~Directory(){}

bool Directory::execute(Command c){
    
    std::string sub = c.pop();
    
    if(directories.count(sub)) return directories[sub]->execute(c);
    else if(functions.count(sub)){
        functions[sub](c);
        return 1;
    }

    return 0;
}

bool Directory::add_directory(std::string key, Directory *directory){
    
    if(key.empty() || key == "." || key == ".." || key == "--" || key == "-") return 0;
    if(directories.count(key)) return 0;
    if(functions.count(key)) return 0;
    
    directories[key] = directory;
    
    return 1;
}

bool Directory::add_function(std::string key, std::function<void(Command)> function){
    
    if(key.empty() || key == "." || key == ".." || key == "--" || key == "-") return 0;
    if(directories.count(key)) return 0;
    if(functions.count(key)) return 0;
    
    functions[key] = function;
    
    return 1;
}

bool Directory::erase_entry(std::string key){
    
    if(directories.count(key)){
        directories.erase(key);
        return 1;
    }
    else if(functions.count(key)){
        functions.erase(key);
        return 1;
    }

    return 0;
}

void Directory::put_directory(std::string key, Directory *directory){
    assert(add_directory(key, directory));
}

void Directory::put_function(std::string key, std::function<void(Command)> function){
    assert(add_function(key, function));
}

void Directory::document(std::string key, std::string doc){
    if(key.empty() || key == "." || key == ".." || key == "--" || key == "-") return;
    docs[key] = doc;
}

Directory *Directory::find(Command c){

    std::string sub = c.pop();
    
    if(sub.empty()) return this;
    else if(directories.count(sub)) return directories[sub]->find(c);
    
    return nullptr;
}

void Directory::print_help(Command c){
    
    std::string subject = c.pop();
    std::string output;
    
    if(subject.empty()){
        for(auto i : docs) output += i.first + ": " + i.second + "\n";
        if(!output.empty()) output.pop_back();
    }
    else {
        output = "no docs found for \"" + subject + "\"";
        if(docs.count(subject)) output = subject + ": " + docs[subject];
    }
    
    c.source.push_output(output);
}

void Directory::print_list(Command c){
    
    std::string list;

    for(auto i : directories) list += i.first + "*\n";
    for(auto i : functions) 
        if(i.first != HELP && i.first != LIST) list += i.first + "\n";
    
    if(list.empty()) list = "(empty)";
    else list.pop_back();
    
    c.source.push_output(list);
}

// terminal ///////////////////////////////////////////////////////////////////

Terminal::Terminal(Window *master_, Kwargs kwargs) :
    Frame(master_, kwargs)
{
    add_function("ls", [&](Command c){ ls(c); });
    add_function("cd", [&](Command c){ cd(c); });
    add_function("pwd", [&](Command c){ pwd(c); });
    add_function("help", [&](Command c){ help(c); });
    add_function("clear", [&](Command c){ clear(c); });

    document("ls", "list sub direcories (dir*) and commands (cmd)");
    document("cd", "(path path path ...). change directory."
            " (..) is the parent directory, (-) is previous and (--) is root.");
    document("pwd", "print working directory.");
    document("help", "(path to ... subject). print docs.");
    document("clear", "clear terminal window.");

    blinker.setString("|");
}

Terminal::~Terminal(){}

Frame::Capture Terminal::on_event(sf::Event event, int32_t priority){
    
    return Capture::pass;
}

void Terminal::set_look(std::string look_){
    
    look = look_;

    Frame::set_look(look);

    text.setFont(font("font"));
    text.setCharacterSize(num("textSize"));
    
    blinker.setFont(font("font"));
    blinker.setCharacterSize(num("textSize"));
    blinker.setFillColor(color("commandColor"));

    set_refresh();
}

void Terminal::on_refresh(){
    
    float charWidth = font("font").getGlyph(0x0020, num("textSize"), 0).advance; // space size
    uint32_t maxChars = std::floor((canvasWidth - offsetX - charWidth) / charWidth);
    
    text.setString(wrap(prefix + edits[editY], maxChars));
    text.setFillColor(color("commandColor"));
    text.setPosition(offsetX, offsetY);
    
    master->draw(text);

    if(edits[editY].empty()) blinker.setPosition(offsetX - charWidth / 2, offsetY);
    else {
        auto [x, y] = text.findCharacterPos(editX + prefix.size());
        blinker.setPosition(x - charWidth / 2, y);
    }

    master->draw(blinker);

    float x = offsetX;
    float y = offsetY + text.getLocalBounds().height;

    uint32_t iter = bufferIndex;
    while(y < canvasHeight && iter < buffer.size()){
        
        text.setString(wrap(buffer[iter].text, maxChars));
        text.setFillColor(color(buffer[iter].color));
        text.setPosition(x, y);

        master->draw(text);

        y += text.getLocalBounds().height;

        iter++;
    }
}

void Terminal::push_output(std::string s){
    buffer.push_front({s, "outputColor"});
    clean_buffer();
}

void Terminal::push_error(std::string s){
    buffer.push_front({s, "errorColor"});
    clean_buffer();
}

void Terminal::push_command(std::string s){
    buffer.push_front({s, "commandColor"});
    clean_buffer();
}

void Terminal::clean_buffer(){
    while(buffer.size() > bufferMax) buffer.pop_back();
    while(edits.size() > bufferMax) edits.pop_back();
    set_refresh();
}

std::vector<std::string> Terminal::create_command(std::string s){

    std::vector<std::string> parsed;
    
    uint32_t left = 0, right = 1;
    while(left < s.size()){
        
        std::string sub;

        if(s[left] == '\"'){
            while(right < s.size() && s[right] != '\"') right++;
            parsed.push_back(s.substr(left + 1, right - left - 1));
            right++;
        }
        else if(s[left] == '\''){
            while(right < s.size() && s[right] != '\'') right++;
            parsed.push_back(s.substr(left + 1, right - left - 1));
            right++;
        }
        else if(left != ' '){
            while(right < s.size() && s[right] != ' ') right++;
            parsed.push_back(s.substr(left, right - left));
        }

        left = right++;
    }

    if(!parsed.empty() && parsed[0] == "."){
       
        std::vector<std::string> old = parsed;
        parsed = address;
        for(uint32_t i=1; i<old.size(); i++) parsed.push_back(old[i]);
        
    }

    return parsed;
}

std::string Terminal::wrap(std::string s, uint32_t max){
    
    std::string wrapped;

    uint32_t length = 0, left = 0, right = 1;
    while(left < s.size()){
        
        while(right < s.size() && s[right] != ' ') right++;
        
        length += right - left;
        
        if(length > max && !wrapped.empty()){
            wrapped += "\n";
            length = right - left;
        }

        wrapped += s.substr(left, right - left);

        left = right++;
    }

    return wrapped;
}

// standard terminal commands /////////////////////////////////////////////////

void Terminal::ls(Command c){

    auto list = address;
    list.push_back(LIST);
    execute({*this, list});

}

void Terminal::cd(Command c){

    std::string dir;
    auto original = address;

    while(!c.empty()){

        if(dir == "-"){
            address = previous;
        }
        else if(dir == ".."){
            if(!address.empty()) address.pop_back();
        }
        else if(dir == "--"){
            address.clear();
        }
        else {
            address.push_back(dir);
            if(find({*this, address}) == nullptr){
                push_error(dir + ": not a directory");
                address = original;
                return;
            }
        }
    }

    previous = original;
}

void Terminal::pwd(Command c){

    std::string list = "--";
    for(auto i : address) list += " " + i;
    push_output(list);

}

void Terminal::help(Command c){
    
    auto list = address;
    list.push_back(HELP);
    list.push_back(c.pop());
    execute({*this, list});

}

void Terminal::clear(Command c){

    buffer.clear();
    bufferIndex = 0;
    set_refresh();

}

}

