#pragma once

#include "app/graph.h"
#include "wave/source.h"
#include "changer/pitch.h"

namespace app {

/*

class Analyzer : public Box {

public:

    enum Mode {
        regularMode,
        frequencyMode,
        peakMode,
        correlationMode
    };
    
    Analyzer(ui::Window *parent_, std::string title_);
    ~Analyzer();
    
    Capture on_event(sf::Event event, int32_t priority);

    int32_t link_audio(std::string fileName);

    int32_t switch_mode(Mode mode);
    
    void update_data();

protected:

    const int32_t defaultLength = 1<<10;

    wave::Source *source = nullptr;
    int32_t position = 0, length = defaultLength;

    Mode dataMode = regularMode;

    ui::Frame frame, analyzerButtons, analyzerWidgets;
    Graph graph;

    ui::Text fileNameBox;
    
    class SR : public ui::Button { using ui::Button::Button; void function(); };
    SR switchRegular;
    class SF : public ui::Button { using ui::Button::Button; void function(); };
    SF switchFrequency;
    class SP : public ui::Button { using ui::Button::Button; void function(); };
    SP switchPeak;
    class AC : public ui::Button { using ui::Button::Button; void function(); };
    AC switchCorrelation;

    wave::Pitch pitch;

};

*/

}
