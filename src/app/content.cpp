#include "app/content.h"
#include "app/app.h"
#include "tools/analyzer.h"
#include "tools/meditor.h"
#include "tools/recorder.h"

namespace app {

Content::Content(App *a, ui::Kwargs kwargs) : ui::Frame(a, kwargs) {}

Content *create_content(std::string type, App *app){
    
    if(type == Analyzer::type) return new Analyzer(app);
    if(type == Meditor::type) return new Meditor(app);
    if(type == Recorder::type) return new Recorder(app);

    return nullptr;
}

bool valid_content_type(std::string type){
    
    auto valids = valid_content_types();
    for(auto i : valids) if(i == type) return 1;

    return 0;
}

std::vector<std::string> valid_content_types(){
    return {
        Analyzer::type,
        Meditor::type,
        Recorder::type
    };
}

}

