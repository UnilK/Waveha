#pragma once

#include "app/box.h"
#include "app/graph.h"
#include "wave/audioFile.h"

class Analyzer : public Box {

public:
    
    Analyzer(ui::Frame *parent_, std::string title_, kwargs values = {});

    bool link_file(std::string fileName);

    int32_t inner_reconfig();
    int32_t draw();

    int32_t execute_command(ui::Command &cmd);

    static int32_t switch_regular(void *analyzer);
    static int32_t switch_frequency(void *analyzer);
    static int32_t switch_peak(void *analyzer);

protected:

    wave::ReadableAudio *source;

    ui::Frame inner, buttonFrame, widgetFrame;
    Graph graph;

    ui::Text fileNameBox;
    ui::Button switchRegular, switchFrequncy, switchPeak;

    enum modes {
        regularMode,
        frequencyMode,
        peakMode
    };

};

