#include "app/analyzer.h"

Analyzer::Analyzer(ui::Frame *parent_, std::string title_, kwargs values) :
    Box(parent_, title_, values),
    inner(this),
    buttonFrame(this),
    widgetFrame(this),
    graph(this),
    fileNameBox(this),
    switchRegular(this, switch_regular, this),
    switchFrequncy(this, switch_frequency, this),
    switchPeak(this, switch_peak, this)
{

}
    
int32_t Analyzer::switch_regular(void *analyzer){

    return 0;
}

int32_t Analyzer::switch_frequency(void *analyzer){

    return 0;
}

int32_t Analyzer::switch_peak(void *analyzer){
    
    return 0;
}

bool Analyzer::link_file(std::string fileName){

    return 1;
}

int32_t Analyzer::inner_reconfig(){

    return 0;
}

int32_t Analyzer::draw(){

    return 0;
}

int32_t Analyzer::execute_command(ui::Command &cmd){

    return -1;
}

