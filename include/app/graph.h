#pragma once

#include "ui/frame.h"

#include "SFML/Graphics/VertexArray.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>


#include <vector>
#include <complex>

struct Point {
    float x;
    std::complex<float> y;
};

class Graph : public ui::ContentFrame {

    /*
       style:
       background (color)
       borderColor (color)
       borderSize (num)
       vertexColor (color)
       axisColor (color)
       indicatorSize (num)
       indicatorColor (color)
       font (font)
       kwargs:
       -
    */

public:

    Graph(ui::Frame *parent_, kwargs values = {});

    int32_t set_look(std::string look_);

    // set offset coordinates for origo (lower left corner).
    void set_origo(float x, float y);
    // set graph scale.
    void set_scale(float x, float y);
    // set scale such that specified area is covered.
    void set_area(float x, float y);

    // turn phase indicator on / off
    void switch_phase_indicator(bool value);
    // turn inspecting too on / off
    void switch_inspection_tool(bool value);
    // stop tracking mouse
    void switch_inspector_lock();

    // equally spaced real valued data
    void set_data(std::vector<float> &data);
    // equally spaced complex valued data
    void set_data(std::vector<std::complex<float> > &data);
    // arbitary points
    void set_data(std::vector<Point> &data);

    // set scale and origo to fit all on x axis.
    void fitX();
    // set only scale to fit all points on y axis.
    void autoscaleY();

    void set_unitX(std::string unit);
    void set_unitY(std::string unit);

    int32_t inner_reconfig();
    int32_t draw();

    void refresh_vertices();
    void refresh_indicator();

    // use for access only
    float origoX = 0, origoY = 0;
    float scaleX = 1, scaleY = 1;

protected:

    bool isComplex = 1;
    std::vector<Point> points;
    sf::VertexArray vertices;

    sf::VertexArray xAxis, yAxis;
    bool yAxisPosition = 0, xAxisPosition = 0;
   
    bool hasPhase = 1;
    bool hasInspector = 1;
    bool inspectorLock = 0;
    float indicatorSize = 0;
    float indicatorA = 0, indicatorX = 0, indicatorY = 0;
    std::string unitX = "", unitY = "";
        
    sf::VertexArray indicatorNeedle;
    sf::VertexArray indicatorLine;
    sf::CircleShape phaseIndicator;
    sf::RectangleShape background;
    sf::Text indicatorTextX;
    sf::Text indicatorTextY;
    sf::Text indicatorTextA;

};

