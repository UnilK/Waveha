#pragma once

#include "ui/frame.h"
#include "app/session.h"

#include <vector>
#include <string>

namespace app {

class App;

class Content : public ui::Frame, public Presistent {

public:

    Content(App*, ui::Kwargs = {});

    virtual std::string content_type() = 0;

};

Content *create_content(std::string type, App *app);

bool valid_content_type(std::string type);

std::vector<std::string> valid_content_types();

}

