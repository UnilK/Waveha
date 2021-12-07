#include "ui/command.h"

#include <iostream>

namespace ui{

const std::string _CMD_LIST_COMMANDS = "cmd";
const std::string _CMD_HELP = "help";
const std::string _CMD_LIST = "ls";
const std::string _CMD_MOVE = "cd";


Command::Command(std::string a, std::string c, bool r, void *p) :
    address(a), command(c), request(r), payload(p) {}



Commandable::Commandable(std::string id_) : id(id_) {}

int32_t Commandable::execute_command(Command &cmd){
    return 0;
}

bool Commandable::execute_standard(Command &cmd){

    std::stringstream cmds(cmd.command);
    
    std::string head;
    cmds >> head;

    if(head == _CMD_HELP){
        std::cout << commandHelp << '\n';
        return 1;
    } else if(head == _CMD_LIST_COMMANDS){
        for(auto i : commandDocs) std::cout << i.first << ": " << i.second << '\n';
        return 1;
    } else if(head == _CMD_LIST){
        for(auto child : commandChildren) std::cout << child->id << '\n';
        return 1;
    } else if(head == _CMD_MOVE){
        
        commandRoot->commandFocus = address;

        std::string location;
        cmds >> location;

        if(location == ".."){
            if(commandParent){
                std::string nextAddress;
                std::getline(cmds, nextAddress);
                Command next("", "cd "+nextAddress);
                return commandParent->execute_standard(next);
            }
        } else {
            for(auto child : commandChildren){
                if(child->id == location){
                    std::string nextAddress;
                    std::getline(cmds, nextAddress);
                    Command next("", "cd "+nextAddress);
                    return child->execute_standard(next);
                }
            }
        }

        return 1;
    }

    return 0;
}

int32_t Commandable::execute_requests(){
    for(Command &cmd : commandRequests) execute_command(cmd);
    commandRequests.clear();
    return 0;
}

int32_t Commandable::update_children(){
    return 0;
}

int32_t Commandable::update_address(std::string a, Commandable *parent_){
    
    address = a;
    if(parent_ == nullptr){
        commandRoot = this;
        commandParent = nullptr;
    } else {
        commandParent = parent_;
        commandRoot = commandParent->commandRoot;
    }

    for(auto &child : commandChildren){
        if(a.empty()) child->update_address(child->id, this);
        else child->update_address(a + " " + child->id, this);
    }

    return 0;
}

int32_t Commandable::deliver_address(Command &cmd){

    if(address == cmd.address){
        if(cmd.request) commandRequests.push_back(cmd);
        else execute_command(cmd);
    } else {
        for(auto &child : commandChildren) child->deliver_address(cmd);
    }

    return 0;
}

int32_t Commandable::deliver_id(Command &cmd){
    
    for(auto &child : commandChildren) child->deliver_id(cmd);
    
    if(id == cmd.address){
        if(cmd.request) commandRequests.push_back(cmd);
        else execute_command(cmd);
    }

    return 0;
}

Command Commandable::create_command(std::stringstream &addressCommand){
    
    std::string next;
    addressCommand >> next;

    for(auto child : commandChildren){
        if(child->id == next) return child->create_command(addressCommand);
    }

    std::string rest;
    std::getline(addressCommand, rest);

    next += rest;

    return Command(address, next);
}

}

