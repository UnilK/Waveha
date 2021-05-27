#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <complex>
#include <iomanip>
#include <cmath>

#include <SFML/Graphics.hpp>

#include "wavestream.h"
#include "FFT.h"

const float PI = 3.14159265;

int main(){

	iwavestream I;
	FFT myFFT;

	std::string file;
	std::cin >> file;

	while(!I.open(file)){
        std::cout << "file not found\n";
        std::cin >> file;
    }

	int B;
    std::cin >> B;

	const int N = 1<<B;
   
    float fileFrequency = I.get_frame_rate();
    float frameHeight = 800, frameWidth = 1400;
    float indicatorSize = 100, graphHeight = frameHeight - 100;

    sf::RenderWindow window(sf::VideoMode(frameWidth, frameHeight),
            "spectrum inspector");

    sf::RectangleShape background({frameWidth, frameHeight});
    background.setFillColor(sf::Color(0x55, 0x55, 0x56));

    sf::CircleShape phaseIndicator(indicatorSize);
    phaseIndicator.setFillColor(sf::Color(0x66, 0x66, 0x67));

    sf::VertexArray graph(sf::LineStrip, N/2);
    sf::VertexArray indicatorNeedle(sf::LineStrip, 2);
    sf::VertexArray indicatorLine(sf::LineStrip, 2);
    
    sf::Color graphColor(0xff, 0xaa, 0xaa);
    sf::Color needleColor(0xaa, 0xff, 0xaa);
    sf::Color lineColor(0xaa, 0xaa, 0xff);

    for(int i=0; i<N/2; i++) graph[i].color = graphColor;
    for(int i=0; i<2; i++) indicatorNeedle[i].color = needleColor;
    for(int i=0; i<2; i++) indicatorLine[i].color = lineColor;
    
    sf::Font font;
    font.loadFromFile("../res/Vogue.ttf");
    sf::Text frequencyIndicator("Frequancy: NaN", font);
    
    frequencyIndicator.setCharacterSize(20);
    frequencyIndicator.setFillColor(needleColor);
    frequencyIndicator.setPosition(20.0f, 20.0f+indicatorSize*2);

    std::vector<float> waves;
    I.read_file(&waves);
    I.close();
   
    float maxd = 1;
    int wpos = 0, ipos = 0, indpos = 0;

    float wip = 0, zoom = 1, magnify = 1;

    float graphLeft = indicatorSize*2 + 50;
    float graphDown = graphHeight + 50;

    std::vector<std::complex<float> > spectrum(N, {0, 0});

    bool graphChanged = 1, viewChanged = 1, indicatorChanged = 1;

    while (window.isOpen()){

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                window.close();
			} else if(event.type == sf::Event::Resized){
				sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			} else if(event.type == sf::Event::KeyPressed){
                if(event.key.code == 0){
                    graphChanged = 1;
                    wpos = std::max(0, wpos-N/2);
                } else if(event.key.code == 3){
                    graphChanged = 1;
                    wpos = std::min((int)waves.size()-N-1, wpos+N/2);
                } else if(event.key.code == 22){
                    viewChanged = 1;
                    zoom *= 1.05;
                } else if(event.key.code == 18){
                    viewChanged = 1;
                    zoom = std::max(1.0, zoom/1.05);
                } else if(event.key.code == 16){
                    viewChanged = 1;
                    wip += 10;
                } else if(event.key.code == 4){
                    viewChanged = 1;
                    wip -= 10;
                } else if(event.key.code == 17){
                    viewChanged = 1;
                    magnify *= 1.1;
                } else if(event.key.code == 5){
                    viewChanged = 1;
                    magnify /= 1.1;
                }

            } else if(event.type == sf::Event::MouseMoved){
                indicatorChanged = 1;
                indpos = event.mouseMove.x;
            }
		}

        if(graphChanged){
            std::vector<float > tmp(N);
            for(int i=0; i<N; i++) tmp[i] = waves[wpos+i];
            spectrum = myFFT.dft(tmp);

            maxd = 0.0;
            for(int i=0; i<N/2; i++) maxd = std::max(maxd, std::abs(spectrum[i]));

            viewChanged = 1;
        }

        if(viewChanged){
            
            for(int i=0; i<N/2; i++){
                
                graph[i].position = sf::Vector2f(graphLeft+i*zoom+wip,
                        (float)graphDown-(std::abs(spectrum[i])/maxd)*graphHeight*magnify);
            }

            indicatorChanged = 1;
        }

        if(indicatorChanged){ 
            
            ipos = std::max(0, std::min(N/2-1,
                      (int)std::round(((indpos-graphLeft-wip)/zoom))));
            
            indicatorNeedle[0].position = sf::Vector2f(indicatorSize, indicatorSize);
            indicatorNeedle[1].position = sf::Vector2f(
                    indicatorSize*(1+std::polar(1.0f, std::arg(spectrum[ipos])).real()),
                    indicatorSize*(1+std::polar(1.0f, std::arg(spectrum[ipos])).imag()));

            indicatorLine[0].position = sf::Vector2f(indpos, graphDown);
            indicatorLine[1].position = sf::Vector2f(indpos, 50);
            
            frequencyIndicator.setString(
                    "Frequency: "+std::to_string((int)std::round(fileFrequency*ipos/N))+" Hz");

        }
    	
        window.clear();
    	window.draw(background);
    	window.draw(phaseIndicator);
        window.draw(graph);
        window.draw(indicatorLine);
        window.draw(frequencyIndicator);
        window.draw(indicatorNeedle);
        window.display();

        graphChanged = 0;
        viewChanged = 0;
        indicatorChanged = 0;

    }

    return 0;
}
