#pragma once

#include "ui/button.h"
#include "ui/slider.h"
#include "ui/terminal.h"
#include "app/session.h"
#include "app/content.h"
#include "app/audio.h"
#include "tools/graph.h"
#include "wave/sound.h"
#include "change/pitch.h"

#include <map>

namespace app {

class App;
class Analyzer;

enum Mode {
    regularMode,
    frequencyMode,
    peakMode,
    correlationMode,
    last
};

class AnalyzerGraph : public Graph {

public:

    AnalyzerGraph(Analyzer&, ui::Kwargs = {});

    void set_look(std::string look_);
    Capture on_event(sf::Event event, int priority);
    void on_refresh();
    
    void save(ui::Saver&);
    void load(ui::Loader&);

    void set_view(Mode);

    friend class Analyzer;

private:

    struct View {
        float origoX = 0, origoY = 0;
        float scaleX = 1, scaleY = 1;
    };
    
    Mode currentMode = regularMode;
    std::map<Mode, View> views;
    
    int clipBegin = 0, clipEnd = 0;
    sf::VertexArray beginLine, endLine;

    Analyzer &analyzer;

};

class Analyzer : public Content {

public:

    
    Analyzer(App*);
    ~Analyzer();

    std::string content_type();

    void save(ui::Saver&);
    void load(ui::Loader&);
    
    void on_tick();

    void switch_mode(Mode mode);
    
    void update_data();
    
    void save_clip();

    friend class AnalyzerGraph;

private:

    App &app;

    void switch_regular();
    void switch_frequency();
    void switch_peak();
    void switch_correlation();

    void link_audio(ui::Command);
    void switch_play(ui::Command);
    void set_name(ui::Command);
    void switch_clip(ui::Command);
    void setup_peaks(ui::Command);
    void setup_correlation(ui::Command);
    void info(ui::Command);

    AudioLink link;
    wave::Player player;
    
    bool clipping = 0;
    int clipBegin = 0, clipEnd = 0;
    std::string clipName;
    const int defaultLength = 1<<10;
    int position = 0, length = defaultLength;

    Mode dataMode = regularMode;

    ui::Slider slider;
    ui::Terminal terminal;
    
    AnalyzerGraph graph;

    ui::Frame buttons;
    ui::Text sourceNameBox;
    ui::Button switchRegular, switchFrequency, switchPeak, switchCorrelation;

    bool playing = 0;

    change::PeakMatchVars peakVars;
    change::CorrelationVars corrVars;

};

}
