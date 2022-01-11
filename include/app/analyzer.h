#pragma once

#include "app/box.h"
#include "app/graph.h"
#include "ui/command.h"
#include "wave/source.h"

namespace app {

class Analyzer : public Box, public ui::Commandable {

public:

    enum Mode {
        regularMode,
        frequencyMode,
        peakMode
    };
    
    Analyzer(ui::Window *parent_, std::string title_);
    ~Analyzer();

    int32_t link_audio(std::string fileName);

    int32_t on_event(sf::Event event, int32_t priority);
    
    int32_t execute_command(ui::Command cmd);

    int32_t switch_mode(Mode mode);
    
    void update_data();

    static int32_t switch_regular(void *analyzer);
    static int32_t switch_frequency(void *analyzer);
    static int32_t switch_peak(void *analyzer);

protected:

    const int32_t defaultLength = 1<<10;

    wave::Source *source = nullptr;
    int32_t position = 0, length = defaultLength;

    Mode dataMode = regularMode;

    ui::Frame frame, analyzerButtons, analyzerWidgets;
    Graph graph;

    ui::Text fileNameBox;
    ui::Button switchRegular, switchFrequency, switchPeak;

};

}
