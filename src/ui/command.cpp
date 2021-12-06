#include "ui/command.h"

namespace ui{


Command::Command(std::string a, std::string c, bool r, void *p) :
    address(a), command(c), request(r), payload(p) {}



Commandable::Commandable(std::string id_) : id(id_) {}

int32_t Commandable::execute_command(Command &cmd){
    return 0;
}

int32_t Commandable::execute_requests(){
    for(Command &cmd : requests) execute_command(cmd);
    requests.clear();
    return 0;
}

int32_t Commandable::update_children(){
    return 0;
}

int32_t Commandable::update_address(std::string a){
    address = a;
    for(auto &child : children) child->update_address(a + " " + id);
    return 0;
}

int32_t Commandable::deliver_address(Command &cmd){

    if(address == cmd.address){
        if(cmd.request) requests.push_back(cmd);
        else execute_command(cmd);
    } else {
        for(auto &child : children) child->deliver_address(cmd);
    }

    return 0;
}

int32_t Commandable::deliver_id(Command &cmd){
    
    for(auto &child : children) child->deliver_id(cmd);
    
    if(id == cmd.address){
        if(cmd.request) requests.push_back(cmd);
        else execute_command(cmd);
    }

    return 0;
}

Command Commandable::create_command(std::stringstream &address_command){
    
    std::string next;
    address_command >> next;

    for(auto child : children){
        if(child->id == next) return child->create_command(address_command);
    }

    std::string rest;
    std::getline(address_command, rest);

    next += rest;

    return Command(address, next);
}

}

