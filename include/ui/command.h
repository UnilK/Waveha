#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace ui{

const std::string _CMD_UPDATE_ADDRESS = "_CMD_UPDATE_ADDRESS";

class Command{

public:

    Command(std::string a, std::string c, bool r = 0, void *p = nullptr);

    std::string address;
    std::string command;
    bool request;
    void *payload; 

};

class Commandable{

public:

    Commandable(std::string id_);

    // command execution
    virtual int32_t execute_command(Command &cmd);
    int32_t execute_requests();

    // update the children of this commandable
    virtual int32_t update_children();

    // update the address of this commandable and it's children.
    int32_t update_address(std::string a);

    // seek the commandable for the command.
    int32_t deliver_address(Command &cmd);
    int32_t deliver_id(Command &cmd);

    // create a command from an address and a command.
    Command create_command(std::stringstream &address_command);

protected:

    // id is the local address.
    std::string address, id;
    std::string help;
    
private:

    // list of tasks given to the commandable.
    std::vector<Command> requests;

    // children of the commandable
    std::vector<Commandable*> children;

};

}

