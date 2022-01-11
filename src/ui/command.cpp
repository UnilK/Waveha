#include "ui/command.h"

#include <iostream>
#include <sstream>

namespace ui {

const std::string _CMD_LIST_COMMANDS = "cmd";
const std::string _CMD_HELP = "help";
const std::string _CMD_LIST = "ls";
const std::string _CMD_MOVE = "cd";


Command::Command(std::string a, std::string c, bool r, void *p) :
    address(a), command(c), request(r), payload(p) {}



Commandable::Commandable(std::string id) : commandId(id) {
    commandFocus = this;
}

Commandable::~Commandable(){
    if(commandRoot != nullptr && commandRoot->commandFocus == this){
        commandRoot->commandFocus = commandRoot;
    }
}

int32_t Commandable::execute_command(Command cmd){
    return 0;
}

bool Commandable::execute_standard(Command cmd){

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

        for(auto child : commandChildren) std::cout << child->commandId << '\n';
        return 1;

    } else if(head == _CMD_MOVE){
        
        commandRoot->commandFocus = this;

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
                if(child->commandId == location){
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
    
void Commandable::link_child(Commandable *child){
    child->commandRoot = commandRoot;
    child->commandParent = this;
    commandChildren.insert(child);
}

void Commandable::detach_child(Commandable *child){
    child->commandRoot = nullptr;
    child->commandParent = nullptr;
    commandChildren.erase(child);
}

void Commandable::clear_children(){
    while(!commandChildren.empty()) detach_child(*commandChildren.begin());
}

int32_t Commandable::deliver_address(Command cmd){

    std::stringstream cin(cmd.address);

    Commandable *current = commandFocus;
    std::string id;

    while(cin >> id) {
        for(auto child : current->commandChildren){
            if(child->commandId == id){
                current = child;
                break;
            }
        }
        if(current->commandId != id) break;
    }

    if(cmd.request) current->commandRequests.push_back(cmd);
    else current->execute_command(cmd);

    return 0;
}

int32_t Commandable::deliver_id(Command cmd){
    
    auto inner = [&](auto &&inner, Commandable *current) -> void {
        for(auto child : current->commandChildren){
            if(child->commandId == cmd.address){
                if(current->commandId == cmd.address){
                    if(cmd.request) current->commandRequests.push_back(cmd);
                    else current->execute_command(cmd);
                }
                else {
                    inner(inner, child);
                }
            }
        }
    };

    inner(inner, commandFocus);

    return 0;
}

Command Commandable::create_command(std::string addressCommand){
    
    std::stringstream cin(addressCommand);
    std::string address, id;
    
    Commandable *current = commandFocus;

    while(cin >> id) {
        for(auto child : current->commandChildren){
            if(child->commandId == id){
                if(address.empty()) address += id;
                else address += " " + id;
                current = child;
                break;
            }
        }
        if(current->commandId != id) break;
    }

    std::string rest;
    std::getline(cin, rest);

    id += rest;

    return Command(address, id);
}

}

