#include "app/content.h"
#include "app/app.h"
#include "tools/analyzer.h"
#include "tools/meditor.h"
#include "tools/recorder.h"
#include "tools/trainer.h"
#include "tools/canvas.h"

namespace app {

namespace Factory {

std::string analyzer = "analyze";
std::string meditor = "medit";
std::string recorder = "record";
std::string trainer = "train";
std::string canvas = "canvas";

}

Content::Content(App *a, ui::Kwargs kwargs) : ui::Frame(a, kwargs) {}

Content *create_content(std::string type, App *app){
    
    if(type == Factory::analyzer) return new Analyzer(app);
    if(type == Factory::meditor) return new Meditor(app);
    if(type == Factory::recorder) return new Recorder(app);
    if(type == Factory::trainer) return new Trainer(app);
    if(type == Factory::canvas) return new CanvasTool(app);

    return nullptr;
}

bool valid_content_type(std::string type){
    
    auto valids = valid_content_types();
    for(auto i : valids) if(i == type) return 1;

    return 0;
}

std::vector<std::string> valid_content_types(){
    return {
        Factory::analyzer,
        Factory::meditor,
        Factory::recorder,
        Factory::canvas,
        Factory::trainer
    };
}

}

