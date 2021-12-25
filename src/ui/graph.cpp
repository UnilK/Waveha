#include "ui/graph.h"

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <math.h>
#include <string>
#include <limits>

namespace ui{

Graph::Graph(Frame *parent_, std::map<std::string, std::string> values) :
    ContentFrame(parent_, values),
    vertices(sf::LineStrip, 0)
{}

void Graph::set_origo(float x, float y){
    origoX = x;
    origoY = y;
    refreshFlag = 1;
}

void Graph::set_scale(float x, float y){
    scaleX = x;
    scaleY = y;
    refreshFlag = 1;
}

void Graph::set_area(float x, float y){
    scaleX = canvasWidth / x;
    scaleY = canvasHeight / y;
    refreshFlag = 1;
}

void Graph::set_phase_indicator(bool value){
    hasPhase = value;
    refreshFlag = 1;
}

void Graph::set_inspection_tool(bool value){
    hasInspector = value;
    refreshFlag = 1;
}

void Graph::switch_inspector_lock(){
    inspectorLock ^= 1;
    refreshFlag = 1;
}

void Graph::set_axis(bool x, bool y){
    hasXAxis = x;
    hasYAxis = y;
    refreshFlag = 1;
}

void Graph::set_axis_position(bool x, bool y){
    hasXAxis = x;
    hasYAxis = y;
    refreshFlag = 1;
}

void Graph::set_data(std::vector<float> &data){
    std::vector<Point> newData(data.size());
    for(uint32_t i=0; i<data.size(); i++) newData[i] = {(float)i, data[i]};
    set_data(newData);
}

void Graph::set_data(std::vector<std::complex<float> > &data){
    std::vector<Point> newData(data.size());
    for(uint32_t i=0; i<data.size(); i++) newData[i] = {(float)i, data[i]};
    set_data(newData);
}

void Graph::set_data(std::vector<Point> &data){
    points = data;
    vertices.resize(points.size());
    refreshFlag = 1;
}

void Graph::fitX(){
    
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    for(Point &i : points){
        min = std::min(min, i.x);
        max = std::max(max, i.x);
    }

    origoX = min;
    scaleX = canvasWidth / (max - min);

    refreshFlag = 1;
}

void Graph::autoscaleY(){

    float max = std::numeric_limits<float>::min();

    for(Point &i : points) max = std::max(max, std::abs(std::abs(i.y) - origoY));

    scaleY = canvasWidth / max;

    refreshFlag = 1;
}

void Graph::set_unitX(std::string unit){
    unitX = unit;
    refreshFlag = 1;
}

void Graph::set_unitY(std::string unit){
    unitY = unit;
    refreshFlag = 1;
}

int32_t Graph::draw(){

    canvas.clear(color("background"));

    float ax = xAxisPosition ? canvasWidth/2 : 0;
    float ay = yAxisPosition ? canvasHeight/2 : canvasHeight;
    
    for(uint32_t i=0; i<points.size(); i++){
        
        vertices[i].color = color("vertexColor");
        
        float x = ax + (points[i].x - origoX) * scaleX;
        float y = ay - (std::abs(points[i].y) - origoY) * scaleY;

        vertices[i].position = sf::Vector2f(x, y);
    }

    canvas.draw(vertices);
    
    sf::VertexArray xAx(sf::LineStrip, 2), yAx(sf::LineStrip, 2);
    
    for(int i=0; i<2; i++) xAx[i].color = color("axisColor");
    for(int i=0; i<2; i++) yAx[i].color = color("axisColor");

    xAx[0].position = sf::Vector2f(0, ax);
    xAx[1].position = sf::Vector2f(canvasWidth, ax);
    
    yAx[0].position = sf::Vector2f(ay, 0);
    yAx[1].position = sf::Vector2f(ay, canvasHeight);

    canvas.draw(xAx);
    canvas.draw(yAx);

    if(hasInspector){

        if(!inspectorLock && contains_mouse()){
            
            indicatorX = local_mouse()[0];

            float x = (indicatorX - ax) / scaleX + origoX;

            float min = std::numeric_limits<float>::max();

            for(Point &i : points){
                if(std::abs(i.x - x) < min){
                    min = std::abs(i.x - x);
                    indicatorX = ax + (i.x - origoX) * scaleX;
                    indicatorA = std::arg(i.y);
                    indicatorY = std::abs(i.y);
                }
            }
        }

        float indicatorSize = num("indicatorSize");

        sf::VertexArray indicatorNeedle(sf::LineStrip, 2);
        sf::VertexArray indicatorLine(sf::LineStrip, 2);
        sf::CircleShape phaseIndicator(indicatorSize);

        phaseIndicator.setFillColor(color("indicatorColor"));
        for(int i=0; i<2; i++) indicatorNeedle[i].color = color("indicatorColor");
        for(int i=0; i<2; i++) indicatorLine[i].color = color("lineColor");

        indicatorLine[0].position = sf::Vector2f(indicatorX, 0);
        indicatorLine[1].position = sf::Vector2f(indicatorX, canvasHeight);

        indicatorNeedle[0] = sf::Vector2f(indicatorSize, indicatorSize);
        indicatorNeedle[1] = sf::Vector2f(
                indicatorSize * (1 + std::polar(1.0f, indicatorA).real()),
                indicatorSize * (1 - std::polar(1.0f, indicatorA).imag()));

        sf::Text indicatorTextX("X: " + std::to_string(indicatorX) + " " + unitX, font("font"));
        sf::Text indicatorTextY("Y: " + std::to_string(indicatorY) + " " + unitY, font("font"));
        sf::Text indicatorTextA("A: " + std::to_string(indicatorA), font("font"));
        
        indicatorTextX.setCharacterSize(indicatorSize - 1);
        indicatorTextY.setCharacterSize(indicatorSize - 1);
        indicatorTextA.setCharacterSize(indicatorSize - 1);
        indicatorTextX.setFillColor(color("indicatorColor"));
        indicatorTextY.setFillColor(color("indicatorColor"));
        indicatorTextA.setFillColor(color("indicatorColor"));
        indicatorTextX.setPosition(2 * indicatorSize + 10, 1);
        indicatorTextY.setPosition(2 * indicatorSize + 10, 2 + indicatorSize);
        indicatorTextA.setPosition(2 * indicatorSize + 10, 3 + 2 * indicatorSize);
        
        canvas.draw(indicatorLine);
        canvas.draw(phaseIndicator);
        canvas.draw(indicatorNeedle);
        canvas.draw(indicatorTextX);
        canvas.draw(indicatorTextY);
        canvas.draw(indicatorTextA);

    }

    display();

    return 0;
}

}
