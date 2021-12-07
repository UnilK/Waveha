#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace ui{

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

    Commandable(std::string id_ = "");
    
    // command execution
    virtual int32_t execute_command(Command &cmd);
    int32_t execute_requests();

    // try standard commands
    bool execute_standard(Command &cmd);

    // update the children of this commandable
    virtual int32_t update_children();

    // update the address and structure of this commandable and it's children.
    int32_t update_address(std::string a, Commandable *parent_ = nullptr);

    // seek the commandable for the command.
    int32_t deliver_address(Command &cmd);
    int32_t deliver_id(Command &cmd);

    // create a command from an address and a command.
    Command create_command(std::stringstream &addressCommand);

protected:

    // id is the local address.
    std::string address, id;
    std::string commandHelp;
    std::vector<std::pair<std::string, std::string> > commandDocs;

    // command focus for the root
    std::string commandFocus;

    // list of tasks given to the commandable.
    std::vector<Command> commandRequests;

    // command structure
    Commandable *commandRoot = nullptr;
    Commandable *commandParent = nullptr;
    std::vector<Commandable*> commandChildren;

};

}

