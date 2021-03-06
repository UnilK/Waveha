#pragma once

#include "ui/frame.h"
#include "app/session.h"

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <vector>
#include <complex>

namespace app {

class App;

struct Point { float x, y, a; };

class Graph : public ui::Frame {

    /*
       style:
       background (color)
       borderColor (color)
       borderThickness (num or num(left) num(right) num(up) num(down))
       vertexColor (color)
       axisColor (color)
       indicatorSize (num)
       indicatorColor (color)
       font (font)
       kwargs:
       -
    */

public:

    Graph(App*, ui::Kwargs = {});
    
    virtual void save(ui::Saver&);
    virtual void load(ui::Loader&);
    
    virtual void set_look(std::string look_);
    virtual Capture on_event(sf::Event event, int priority);
    virtual void on_reconfig();
    virtual void on_refresh();
    
    // NOTE: since most of these operations are rather heavy,
    // they don't update the plot automatically. The plot should be
    // updated manually by setting the reconfig flag, set_reconfig().

    // set offset coordinates for origo (lower left corner).
    void set_origo(float x, float y);
    // set graph scale.
    void set_scale(float x, float y);
    void scale(float x, float y);
    // set scale such that specified area is covered.
    void set_area(float x, float y);

    // set origo location relative to screen dimesions. x, y are in [0, 1]
    void set_relative_origo(float x, float y);

    // turn phase indicator on / off
    void switch_phase_indicator(bool value);
    // turn inspecting too on / off
    void switch_inspection_tool(bool value);

    // equally spaced real valued data
    void set_data(const std::vector<float> &data);
    // equally spaced complex valued data
    void set_data(const std::vector<std::complex<float> > &data);
    // arbitary points
    void set_data(const std::vector<Point> &data);

    // set scale and origo to fit all on axis.
    void fit_x();
    void fit_y();

    // manipulate indicator text.
    void set_unit_x(std::string unit);
    void set_unit_y(std::string unit);
    void set_offset_x(double);
    void set_offset_y(double);
    void set_scalar_x(double);
    void set_scalar_y(double);

    // use for access only
    float origoX = 0, origoY = 0;
    float scaleX = 1, scaleY = 1;

protected:

    std::vector<Point> points;
    sf::VertexArray vertices;

    sf::VertexArray xAxis, yAxis;
    bool yAxisPosition = 0, xAxisPosition = 0;
  
    // for moving the graph with a mouse
    float dragX = 0, dragY = 0;
    float beginX = 0, beginY = 0;

    // offsets to indicator text values.
    double offsetX = 0, offsetY = 0;
    double scalarX = 1, scalarY = 1;

    // indicator stuff
    bool graphPressed = 0;
    bool hasPhase = 1;
    bool hasText = 1;
    bool hasInspector = 1;
    float indicatorSize = 0;
    double indicatorA = 0, indicatorX = 0, indicatorY = 0;
    std::string unitX = "", unitY = "";

    sf::VertexArray indicatorNeedle;
    sf::VertexArray indicatorLine;
    sf::CircleShape phaseIndicator;
    sf::Text indicatorTextX;
    sf::Text indicatorTextY;
    sf::Text indicatorTextA;

};

}
