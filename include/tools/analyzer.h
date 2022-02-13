#pragma once

#include "ui/button.h"
#include "ui/slider.h"
#include "ui/terminal.h"
#include "app/session.h"
#include "app/slot.h"
#include "tools/graph.h"
#include "wave/source.h"
#include "change/pitch.h"

namespace app {

class App;
class Analyzer;

class AnalyzerGraph : public Graph {

public:

    AnalyzerGraph(Analyzer&, ui::Kwargs = {});

    Capture on_event(sf::Event event, int priority);

private:

    Analyzer &analyzer;

};

class Analyzer : public Content {

public:

    enum Mode {
        regularMode,
        frequencyMode,
        peakMode,
        correlationMode
    };
    
    Analyzer(App*);
    ~Analyzer();

    std::string content_type();

    void save(Saver&);
    void load(Loader&);

    void switch_mode(Mode mode);
    
    void update_data();

    friend class AnalyzerGraph;

private:

    static const std::string type;
    static int init_class;

    App &app;

    void switch_regular();
    void switch_frequency();
    void switch_peak();
    void switch_correlation();

    void link_audio(ui::Command);

    const int defaultLength = 1<<10;

    std::string sourceName = "";
    wave::Source *source = nullptr;

    int position = 0, length = defaultLength;

    Mode dataMode = regularMode;

    change::Pitch pitch;

    ui::Slider slider;
    ui::Terminal terminal;
    
    AnalyzerGraph graph;
    
    ui::Frame buttons;
    ui::Text sourceNameBox;
    ui::Button switchRegular, switchFrequency, switchPeak, switchCorrelation;

};

}
