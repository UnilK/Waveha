#include "tools/graph.h"
#include "app/app.h"
#include "math/constants.hpp"

#include <iostream>
#include <math.h>
#include <string>
#include <limits>

namespace app {

Graph::Graph(App *app, ui::Kwargs kwargs) :
    Content(app, kwargs),
    vertices(sf::LineStrip, 0),
    xAxis(sf::LineStrip, 2),
    yAxis(sf::LineStrip, 2),
    indicatorNeedle(sf::LinesStrip, 2),
    indicatorLine(sf::LineStrip, 2)
{
    set_look(look);
}

void Graph::save(Saver &saver){
    
    saver.write_float(origoX);
    saver.write_float(origoY);
    saver.write_float(scaleX);
    saver.write_float(scaleY);
}

void Graph::load(Loader &loader){
    
    origoX = loader.read_float();
    origoY = loader.read_float();
    scaleX = loader.read_float();
    scaleY = loader.read_float();

    refresh_all();
}

std::string Graph::content_type(){ return "graph"; }

void Graph::set_look(std::string look_){
   
    Frame::set_look(look_);

    indicatorSize = num("indicatorSize");

    for(int i=0; i<2; i++) xAxis[i].color = color("axisColor");
    for(int i=0; i<2; i++) yAxis[i].color = color("axisColor");
   
    phaseIndicator.setRadius(indicatorSize);
    phaseIndicator.setFillColor(sf::Color(0, 0, 0, 0));
    phaseIndicator.setOutlineThickness(-1);
    phaseIndicator.setOutlineColor(color("indicatorColor"));
    for(int i=0; i<2; i++) indicatorNeedle[i].color = color("indicatorColor");
    for(int i=0; i<2; i++) indicatorLine[i].color = color("indicatorColor");
    
    indicatorTextX.setCharacterSize(indicatorSize);
    indicatorTextY.setCharacterSize(indicatorSize);
    indicatorTextA.setCharacterSize(indicatorSize);
    indicatorTextX.setFillColor(color("indicatorColor"));
    indicatorTextY.setFillColor(color("indicatorColor"));
    indicatorTextA.setFillColor(color("indicatorColor"));
    indicatorTextX.setPosition(2 * indicatorSize + 5, 0);
    indicatorTextY.setPosition(2 * indicatorSize + 5, indicatorSize);
    indicatorTextA.setPosition(2 * indicatorSize + 5, 2 * indicatorSize);
    indicatorTextX.setFont(font("font"));
    indicatorTextY.setFont(font("font"));
    indicatorTextA.setFont(font("font"));
    
    indicatorLine[0].position = sf::Vector2f(0, 0);
    indicatorLine[1].position = sf::Vector2f(0, canvasHeight);

    indicatorNeedle[0].position = sf::Vector2f(indicatorSize, indicatorSize);
    indicatorNeedle[1].position = sf::Vector2f(2 * indicatorSize, indicatorSize);

    xAxis[0].position = sf::Vector2f(0, 0);
    xAxis[1].position = sf::Vector2f(canvasWidth, 0);
    
    yAxis[0].position = sf::Vector2f(0, 0);
    yAxis[1].position = sf::Vector2f(0, canvasHeight);

    refresh_vertices();
    refresh_indicator();
}

ui::Frame::Capture Graph::on_event(sf::Event event, int32_t priority){

    if(priority > 0) return Capture::pass;

    if(event.type == sf::Event::MouseButtonPressed){
        
        if(event.mouseButton.button == sf::Mouse::Left){
            auto [x, y] = global_mouse();
            dragX = x;
            dragY = y;

            beginX = origoX;
            beginY = origoY;
            
            graphPressed = 1;

            return Capture::capture;
        }

    }
    else if(event.type == sf::Event::MouseButtonReleased){

        if(event.mouseButton.button == sf::Mouse::Left){
            graphPressed = 0;
            return Capture::capture;
        }

    }
    else if(event.type == sf::Event::MouseMoved){
        
        if(graphPressed){
            
            auto [x, y] = global_mouse();

            float newX = beginX - (x - dragX) / scaleX;
            float newY = beginY + (y - dragY) / scaleY;

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
                newX = origoX;
            else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                newY = origoY;

            set_origo(newX, newY);
            refresh_vertices();
        }

        refresh_indicator();

        return Capture::capture;

    }
    else if(event.type == sf::Event::MouseWheelScrolled){
       
        if(priority < 0) return Capture::pass;

        float scrollSpeed = 1.1f;
        if(event.mouseWheelScroll.delta < 0) scrollSpeed = 1 / scrollSpeed;

        float scrollX = scrollSpeed, scrollY = scrollSpeed;

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            scrollX = 1;
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            scrollY = 1;

        auto [x, y] = local_mouse();

        float centerX = origoX + (x / scaleX);
        float centerY = origoY + ((canvasHeight - y) / scaleY);

        scaleX *= scrollX;
        scaleY *= scrollY;

        origoX = centerX - (x / scaleX);
        origoY = centerY - ((canvasHeight - y) / scaleY);

        refresh_all();

        return Capture::capture;
    }
    else if(event.type == sf::Event::KeyReleased){
        
        if(event.key.code == sf::Keyboard::X){
            fit_x();
            refresh_all();
            return Capture::capture;
        }
        else if(event.key.code == sf::Keyboard::Y){
            fit_y();
            refresh_all();
            return Capture::capture;
        }

    }

    return Capture::pass;
}

void Graph::set_origo(float x, float y){
    origoX = x;
    origoY = y;
}

void Graph::set_scale(float x, float y){
    scaleX = x;
    scaleY = y;
}

void Graph::scale(float x, float y){
    scaleX *= x;
    scaleY *= y;
}

void Graph::set_area(float x, float y){
    scaleX = canvasWidth / x;
    scaleY = canvasHeight / y;
}

void Graph::set_relative_origo(float x, float y){
    origoY = canvasHeight / scaleY * (1 - y);
    origoX = canvasWidth / scaleX * x;
}

void Graph::switch_phase_indicator(bool value){
    hasPhase = value;
}

void Graph::switch_inspection_tool(bool value){
    hasInspector = value;
}

void Graph::switch_inspector_lock(){
    inspectorLock ^= 1;
}

void Graph::set_data(const std::vector<float> &data, bool imag){
    std::vector<Point> newData(data.size());
    for(uint32_t i=0; i<data.size(); i++) newData[i] = {(float)i, data[i]};
    set_data(newData, imag);
}

void Graph::set_data(const std::vector<std::complex<float> > &data, bool imag){
    std::vector<Point> newData(data.size());
    for(uint32_t i=0; i<data.size(); i++) newData[i] = {(float)i, data[i]};
    set_data(newData, imag);
}

void Graph::set_data(const std::vector<Point> &data, bool imag){
    points = data;
    isComplex = imag;
    vertices.resize(points.size());
}

void Graph::fit_x(){
    
    float min = 0;
    float max = 0;

    for(Point &i : points){
        min = std::min(min, i.x);
        max = std::max(max, i.x);
    }
    
    if(min == max) max = min + 1;

    origoX = min;
    scaleX = canvasWidth / (max - min);
}

void Graph::fit_y(){

    float min = 0;
    float max = 0;

    for(Point &i : points){
        float y = std::abs(i.y);
        if(!isComplex && std::abs(std::arg(i.y)) > PI / 2) y = -y;
        min = std::min(min, y);
        max = std::max(max, y);
    }
   
    if(min == max) max = min + 1;

    scaleY = canvasHeight / (max - min) * 0.9f;
    origoY = min - (max - min) * 0.05f;
}

void Graph::set_unit_x(std::string unit){
    unitX = unit;
}

void Graph::set_unit_y(std::string unit){
    unitY = unit;
}
    
void Graph::set_offset_x(double x){
    offsetX = x;
}

void Graph::set_offset_y(double y){
    offsetY = y;
}

void Graph::set_scalar_x(double x){
    scalarX = x;
}

void Graph::set_scalar_y(double y){
    scalarY = y;
}

void Graph::on_reconfig(){
    border.set_size(canvasWidth, canvasHeight);
    refresh_all();  
}

void Graph::refresh_vertices(){

    vertices.resize(points.size());

    for(uint32_t i=0; i<points.size(); i++){
       
        vertices[i].color = color("vertexColor");
        
        float x = (points[i].x - origoX) * scaleX;
        float y = std::abs(points[i].y);
        if(!isComplex && std::abs(std::arg(points[i].y)) > PIF / 2) y = -y;
        y = canvasHeight - (y - origoY) * scaleY;

        vertices[i].position = sf::Vector2f(x, y);
    }
    
    xAxis[0].position = sf::Vector2f(0, canvasHeight + origoY * scaleY);
    xAxis[1].position = sf::Vector2f(canvasWidth, canvasHeight + origoY * scaleY);
    
    yAxis[0].position = sf::Vector2f(-origoX * scaleX, 0);
    yAxis[1].position = sf::Vector2f(-origoX * scaleX, canvasHeight);

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
            indicatorSize * (1 + std::polar(1.0, indicatorA).real()),
            indicatorSize * (1 - std::polar(1.0, indicatorA).imag()));

    indicatorTextX.setString(
            "X: " + std::to_string((indicatorX / scaleX + origoX) * scalarX + offsetX)
            + " " + unitX);

    indicatorTextY.setString("Y: " + std::to_string(indicatorY * scalarY + offsetY) + " " + unitY);
    indicatorTextA.setString("A: " + std::to_string(indicatorA));

    set_refresh();
}

void Graph::refresh_all(){
    refresh_vertices();
    refresh_indicator();
}

void Graph::on_refresh(){

    Frame::on_refresh();

    master->draw(xAxis);
    master->draw(yAxis);
    master->draw(vertices);

    if(hasInspector){

        master->draw(indicatorLine);
        master->draw(phaseIndicator);
        master->draw(indicatorNeedle);
        master->draw(indicatorTextX);
        master->draw(indicatorTextY);
        master->draw(indicatorTextA);

    }
}

}
