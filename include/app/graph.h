#pragma once

#include "ui/frame.h"

#include "SFML/Graphics/VertexArray.hpp"

#include <vector>
#include <complex>

struct Point {
    float x;
    std::complex<float> y;
};

class Graph : public ui::ContentFrame {

    /*
       style parameters:
       background (color)
       vertexColor (color)
       axisColor (color)
       indicatorSize (num)
       indicatorColor (color)
       font (font)
    */

public:

    Graph(ui::Frame *parent_, kwargs values = {});

    // set offset coordinates for origo (lower left corner).
    void set_origo(float x, float y);

    // set graph scale.
    void set_scale(float x, float y);

    // set scale such that specified area is covered.
    void set_area(float x, float y);

    // turn phase indicator on / off
    void set_phase_indicator(bool value);

    // turn inspecting too on / off
    void set_inspection_tool(bool value);

    // stop tracking mouse
    void switch_inspector_lock();

    // turn axis on / off
    void set_axis(bool x, bool y);

    // set location of axises : 0 bottom or 1 middle
    void set_axis_position(bool x, bool y);

    // equally spaced real valued data
    void set_data(std::vector<float> &data);
    
    // equally spaced complex valued data
    void set_data(std::vector<std::complex<float> > &data);
    
    // arbitary points
    void set_data(std::vector<Point> &data);

    // set scale and origo to fit all on x axis.
    void fitX();
    
    // set scale to fit all points on y axis
    void autoscaleY();

    void set_unitX(std::string unit);
    void set_unitY(std::string unit);

    int32_t draw();

    float origoX = 0, origoY = 0;
    float scaleX = 1, scaleY = 1;

protected:

    std::string unitX = "", unitY = "";

    bool isComplex = 1;
    std::vector<Point> points;

    sf::VertexArray vertices;

    bool hasPhase = 1;
    bool hasInspector = 1;
    bool hasXAxis = 1, hasYAxis = 1;
    bool yAxisPosition = 0, xAxisPosition = 0;
   
    bool inspectorLock = 0;
    float indicatorA = 0, indicatorX = 0, indicatorY = 0;

};

