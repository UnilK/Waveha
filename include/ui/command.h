#pragma once

#include <string>
#include <vector>
#include <set>

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

    Commandable(std::string id = "_");
    virtual ~Commandable();

    // command execution
    virtual int32_t execute_command(Command cmd);
    int32_t execute_requests();

    // try standard commands
    bool execute_standard(Command cmd);

    // link commandables
    void link_child(Commandable *child);
    void detach_child(Commandable *child);
    void clear_children();

    // seek the commandable for the command.
    int32_t deliver_address(Command cmd);
    int32_t deliver_id(Command cmd);

    // create a command from an address and a command.
    Command create_command(std::string addressCommand);

protected:

    std::string commandId;
    std::string commandHelp;
    std::vector<std::pair<std::string, std::string> > commandDocs;

    // command focus for the root
    Commandable *commandFocus;

    // list of tasks given to the commandable.
    std::vector<Command> commandRequests;

    // command structure
    Commandable *commandRoot = nullptr;
    Commandable *commandParent = nullptr;
    std::set<Commandable*> commandChildren;

};

}

