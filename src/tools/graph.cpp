#include "tools/graph.h"
#include "app/app.h"
#include "math/constants.h"

#include <iostream>
#include <math.h>
#include <string>
#include <limits>

namespace app {

Graph::Graph(App *app, ui::Kwargs kwargs) :
    ui::Frame(app, kwargs),
    vertices(sf::LineStrip, 0),
    xAxis(sf::LineStrip, 2),
    yAxis(sf::LineStrip, 2),
    indicatorNeedle(sf::LinesStrip, 2),
    indicatorLine(sf::LineStrip, 2)
{
    set_look(look);
}

void Graph::save(ui::Saver &saver){
    
    saver.write_float(origoX);
    saver.write_float(origoY);
    saver.write_float(scaleX);
    saver.write_float(scaleY);
}

void Graph::load(ui::Loader &loader){
    
    origoX = loader.read_float();
    origoY = loader.read_float();
    scaleX = loader.read_float();
    scaleY = loader.read_float();

    set_reconfig();
}

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

    set_reconfig();
}

ui::Frame::Capture Graph::on_event(sf::Event event, int priority){

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
        }
        
        set_reconfig();

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

        set_reconfig();

        return Capture::capture;
    }
    else if(event.type == sf::Event::KeyReleased){
        
        if(event.key.code == sf::Keyboard::X){
            fit_x();
            set_reconfig();
            return Capture::capture;
        }
        else if(event.key.code == sf::Keyboard::Y){
            fit_y();
            set_reconfig();
            return Capture::capture;
        }
    }
    
    return Capture::pass;
}

void Graph::on_reconfig(){
    
    Frame::on_reconfig();
    
    vertices.resize(points.size());

    sf::Vertex origo(
            sf::Vector2f( -origoX * scaleX, canvasHeight + origoY * scaleY),
            color("vertexColor"));    

    float mouseX = local_mouse()[0] / scaleX + origoX;

    float min = std::numeric_limits<float>::max();

    for(unsigned i=0; i<points.size(); i++){

        vertices[i] = origo;
        vertices[i].position.x += points[i].x * scaleX;
        vertices[i].position.y -= points[i].y * scaleY;
        
        if(std::abs(points[i].x - mouseX) < min){
            min = std::abs(points[i].x - mouseX);
            indicatorX = points[i].x;
            indicatorY = points[i].y;
            indicatorA = points[i].a;
        }
    }
    
    xAxis[0].position = sf::Vector2f(0, canvasHeight + origoY * scaleY);
    xAxis[1].position = sf::Vector2f(canvasWidth, canvasHeight + origoY * scaleY);
    
    yAxis[0].position = sf::Vector2f(-origoX * scaleX, 0);
    yAxis[1].position = sf::Vector2f(-origoX * scaleX, canvasHeight);


    indicatorLine[0].position = sf::Vector2f((indicatorX - origoX) * scaleX, 0);
    indicatorLine[1].position = sf::Vector2f((indicatorX - origoX) * scaleX, canvasHeight);

    indicatorNeedle[0].position = sf::Vector2f(indicatorSize, indicatorSize);
    indicatorNeedle[1].position = sf::Vector2f(
            indicatorSize * (1 + std::polar(1.0, indicatorA).real()),
            indicatorSize * (1 - std::polar(1.0, indicatorA).imag()));

    indicatorTextX.setString(
            "X: " + std::to_string(indicatorX * scalarX + offsetX)
            + " " + unitX);

    indicatorTextY.setString("Y: " + std::to_string(indicatorY * scalarY + offsetY) + " " + unitY);
    indicatorTextA.setString("A: " + std::to_string(indicatorA));

    set_refresh();
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

        if(hasText){
            master->draw(indicatorTextX);
            master->draw(indicatorTextY);
            master->draw(indicatorTextA);
        }
    }
}

void Graph::set_origo(float x, float y){
    origoX = x;
    origoY = y;
    set_reconfig();
}

void Graph::set_scale(float x, float y){
    scaleX = x;
    scaleY = y;
    set_reconfig();
}

void Graph::scale(float x, float y){
    scaleX *= x;
    scaleY *= y;
    set_reconfig();
}

void Graph::set_area(float x, float y){
    scaleX = canvasWidth / x;
    scaleY = canvasHeight / y;
    set_reconfig();
}

void Graph::set_relative_origo(float x, float y){
    origoY = canvasHeight / scaleY * (1 - y);
    origoX = canvasWidth / scaleX * x;
    set_reconfig();
}

void Graph::switch_phase_indicator(bool value){
    hasPhase = value;
    set_reconfig();
}

void Graph::switch_inspection_tool(bool value){
    hasInspector = value;
    set_reconfig();
}

void Graph::set_data(const std::vector<float> &data){
    points.resize(data.size());
    for(unsigned i=0; i<data.size(); i++) points[i] = {(float)i, data[i], 0};
    vertices.resize(points.size());
    set_reconfig();
}

void Graph::set_data(const std::vector<std::complex<float> > &data){
    points.resize(data.size());
    for(unsigned i=0; i<data.size(); i++){
        points[i] = {(float)i, std::abs(data[i]), std::arg(data[i])};
    }
    vertices.resize(points.size());
    set_reconfig();
}

void Graph::set_data(const std::vector<Point> &data){
    points = data;
    vertices.resize(points.size());
    set_reconfig();
}

void Graph::fit_x(){
    
    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::min();

    for(Point &i : points){
        min = std::min(min, i.x);
        max = std::max(max, i.x);
    }
    
    if(min == max) max = min + 1;

    origoX = min;
    scaleX = canvasWidth / (max - min);
    if(scaleX == 0.0f) scaleX = 1.0f;
    set_reconfig();
}

void Graph::fit_y(){

    float min = 0;
    float max = 0;

    for(Point &i : points){
        min = std::min(min, i.y);
        max = std::max(max, i.y);
    }
   
    if(min == max) max = min + 1;

    scaleY = canvasHeight / (max - min) * 0.9f;

    if(scaleY == 0.0f) scaleY = 1.0f;

    origoY = min - (max - min) * 0.05f;
    set_reconfig();
}

void Graph::set_unit_x(std::string unit){
    unitX = unit;
    set_reconfig();
}

void Graph::set_unit_y(std::string unit){
    unitY = unit;
    set_reconfig();
}
    
void Graph::set_offset_x(double x){
    offsetX = x;
    set_reconfig();
}

void Graph::set_offset_y(double y){
    offsetY = y;
    set_reconfig();
}

void Graph::set_scalar_x(double x){
    scalarX = x;
    set_reconfig();
}

void Graph::set_scalar_y(double y){
    scalarY = y;
    set_reconfig();
}

}
