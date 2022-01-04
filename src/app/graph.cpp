#include "app/graph.h"

#include <math.h>
#include <string>
#include <limits>

Graph::Graph(Frame *parent_, kwargs values) :
    ContentFrame(parent_, {{"look", "Graph"}}),
    vertices(sf::LineStrip, 0)
{

    vertices.setPrimitiveType(sf::LineStrip);

    indicatorNeedle.setPrimitiveType(sf::LineStrip);
    indicatorLine.setPrimitiveType(sf::LineStrip);
    indicatorNeedle.resize(2);
    indicatorLine.resize(2);

    set_look(look);
}

int32_t Graph::set_look(std::string look_){
   
    look = look_;

    indicatorSize = num("indicatorSize");

    for(int i=0; i<2; i++) xAxis[i].color = color("axisColor");
    for(int i=0; i<2; i++) yAxis[i].color = color("axisColor");
   
    background.setFillColor(color("background"));
    background.setOutlineColor(color("borderColor"));
    background.setOutlineThickness(num("borderThickness"));

    phaseIndicator.setRadius(indicatorSize);
    phaseIndicator.setFillColor(sf::Color(0, 0, 0, 0));
    phaseIndicator.setOutlineColor(color("indicatorColor"));
    for(int i=0; i<2; i++) indicatorNeedle[i].color = color("indicatorColor");
    for(int i=0; i<2; i++) indicatorLine[i].color = color("lineColor");
    
    indicatorTextX.setCharacterSize(indicatorSize);
    indicatorTextY.setCharacterSize(indicatorSize);
    indicatorTextA.setCharacterSize(indicatorSize);
    indicatorTextX.setFillColor(color("indicatorColor"));
    indicatorTextY.setFillColor(color("indicatorColor"));
    indicatorTextA.setFillColor(color("indicatorColor"));
    indicatorTextX.setPosition(2 * indicatorSize + 5, 0);
    indicatorTextY.setPosition(2 * indicatorSize + 5, indicatorSize);
    indicatorTextA.setPosition(2 * indicatorSize + 5, 2 * indicatorSize);
    
    indicatorLine[0].position = sf::Vector2f(0, 0);
    indicatorLine[1].position = sf::Vector2f(0, canvasHeight);

    indicatorNeedle[0].position = sf::Vector2f(indicatorSize, indicatorSize);
    indicatorNeedle[1].position = sf::Vector2f(2 * indicatorSize, indicatorSize);

    refresh_vertices();
    refresh_indicator();

    return 0;
}

void Graph::set_origo(float x, float y){
    origoX = x;
    origoY = y;
    refresh_vertices();
    refresh_indicator();
}

void Graph::set_scale(float x, float y){
    scaleX = x;
    scaleY = y;
    refresh_vertices();
    refresh_indicator();
}

void Graph::set_area(float x, float y){
    scaleX = canvasWidth / x;
    scaleY = canvasHeight / y;
    refresh_vertices();
    refresh_indicator();
}

void Graph::switch_phase_indicator(bool value){
    hasPhase = value;
    refresh_indicator();
}

void Graph::switch_inspection_tool(bool value){
    hasInspector = value;
    refresh_indicator();
}

void Graph::switch_inspector_lock(){
    inspectorLock ^= 1;
    refresh_indicator();
}

void Graph::set_data(std::vector<float> &data){
    std::vector<Point> newData(data.size());
    for(uint32_t i=0; i<data.size(); i++) newData[i] = {(float)i, data[i]};
    set_data(newData);
    refresh_vertices();
    refresh_indicator();
}

void Graph::set_data(std::vector<std::complex<float> > &data){
    std::vector<Point> newData(data.size());
    for(uint32_t i=0; i<data.size(); i++) newData[i] = {(float)i, data[i]};
    set_data(newData);
    refresh_vertices();
    refresh_indicator();
}

void Graph::set_data(std::vector<Point> &data){
    points = data;
    vertices.resize(points.size());
    refresh_vertices();
    refresh_indicator();
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

    refresh_vertices();
    refresh_indicator();
}

void Graph::autoscaleY(){

    float max = std::numeric_limits<float>::min();

    for(Point &i : points) max = std::max(max, std::abs(std::abs(i.y) - origoY));

    scaleY = canvasWidth / max;

    refresh_vertices();
    refresh_indicator();
}

void Graph::set_unitX(std::string unit){
    unitX = unit;
    refresh_indicator();
}

void Graph::set_unitY(std::string unit){
    unitY = unit;
    refresh_indicator();
}

int32_t Graph::inner_reconfig(){

    background.setSize({canvasWidth, canvasHeight});
    refresh_indicator();
   
    return 0;
}

void Graph::refresh_vertices(){

    vertices.resize(points.size());

    for(uint32_t i=0; i<points.size(); i++){
        
        vertices[i].color = color("vertexColor");
        
        float x = (points[i].x - origoX) * scaleX;
        float y = (std::abs(points[i].y) - origoY) * scaleY;

        vertices[i].position = sf::Vector2f(x, y);
    }

    set_refresh();
}

void Graph::refresh_indicator(){

    if(!inspectorLock && contains_mouse()){
        
        indicatorX = local_mouse()[0];

        float x = indicatorX / scaleX + origoX;

        float min = std::numeric_limits<float>::max();

        for(Point &i : points){
            if(std::abs(i.x - x) < min){
                min = std::abs(i.x - x);
                indicatorX = (i.x - origoX) * scaleX;
                indicatorA = std::arg(i.y);
                indicatorY = std::abs(i.y);
            }
        }
    }

    indicatorLine[0].position = sf::Vector2f(indicatorX, 0);
    indicatorLine[1].position = sf::Vector2f(indicatorX, canvasHeight);

    indicatorNeedle[0].position = sf::Vector2f(indicatorSize, indicatorSize);
    indicatorNeedle[1].position = sf::Vector2f(
            indicatorSize * (1 + std::polar(1.0f, indicatorA).real()),
            indicatorSize * (1 - std::polar(1.0f, indicatorA).imag()));

    indicatorTextX.setString("X: " + std::to_string(indicatorX) + " " + unitX);
    indicatorTextY.setString("Y: " + std::to_string(indicatorY) + " " + unitY);
    indicatorTextA.setString("A: " + std::to_string(indicatorA));

    set_refresh();
}

int32_t Graph::draw(){

    canvas.clear(color("background"));

    canvas.draw(vertices);

    if(hasInspector){

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

