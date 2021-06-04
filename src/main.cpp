#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <complex>
#include <iomanip>
#include <cmath>
#include <thread>
#include <chrono>

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

	int N = 1<<10;
   
    float fileFrequency = I.get_frame_rate();
    float frameHeight = 800, frameWidth = 1400;
    float indicatorSize = 100, graphHeight = frameHeight - 100;

    sf::RenderWindow window(sf::VideoMode(frameWidth, frameHeight), "wave analyzer");

    sf::RectangleShape background({frameWidth, frameHeight});
    background.setFillColor(sf::Color(0x55, 0x55, 0x56));

    sf::CircleShape phaseIndicator(indicatorSize);
    phaseIndicator.setFillColor(sf::Color(0x66, 0x66, 0x67));

    sf::VertexArray graph(sf::LineStrip, N);
    sf::VertexArray indicatorNeedle(sf::LineStrip, 2);
    sf::VertexArray indicatorLine(sf::LineStrip, 2);
    
    sf::Color graphColor(0xff, 0xaa, 0xaa);
    sf::Color needleColor(0xaa, 0xff, 0xaa);
    sf::Color lineColor(0xaa, 0xaa, 0xff);

    for(int i=0; i<N; i++) graph[i].color = graphColor;
    for(int i=0; i<2; i++) indicatorNeedle[i].color = needleColor;
    for(int i=0; i<2; i++) indicatorLine[i].color = lineColor;
    
    sf::Font font;
    font.loadFromFile("../res/fonts/UbuntuMono-R.ttf");
    sf::Text frequencyIndicator("Frequancy: NaN", font);
    
    frequencyIndicator.setCharacterSize(20);
    frequencyIndicator.setFillColor(needleColor);
    frequencyIndicator.setPosition(20.0f, 20.0f+indicatorSize*2);

    std::vector<float> waves;
    I.read_file(&waves);
    I.close();
   
    int wpos = 0, ipos = 0, indpos = 0;

    float graphLeft = indicatorSize*2 + 50;
    float graphDown = graphHeight + 50;

    std::vector<std::complex<float> > spectrum(N, {0, 0});

    bool sizeChanged = 0, graphChanged = 1, viewChanged = 1, indicatorChanged = 1, redraw = 1;
    
    int windowMode = 0;
    int analyses = 3, analysisMode = 0;

    float magnify[3] = {1, 10, 40};
    float zoom[3] = {1, 1, 1};
    float wip[3] = {0, 0, 0};

    float fundamental = 0, maxFundamental = 800;

    while (window.isOpen()){

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed){
                window.close();
			} else if(event.type == sf::Event::Resized){
				
                frameHeight = event.size.height;
                frameWidth = event.size.width;
                graphHeight = frameHeight - 100;
                graphDown = graphHeight + 50;

                sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
                
                sf::Vector2f newSize(frameWidth, frameHeight);
                background.setSize(newSize);

				window.setView(sf::View(visibleArea));

                graphChanged = 1;

			} else if(event.type == sf::Event::KeyPressed){
                
                switch(event.key.code){
                    case 0:
                        graphChanged = 1;
                        wpos = std::max(0, wpos-N/2);
                        break;
                    case 3:
                        graphChanged = 1;
                        wpos = std::min((int)waves.size()-N-1, wpos+N/2);
                        break;
                    case 22:
                        viewChanged = 1;
                        wip[analysisMode] /= zoom[analysisMode];
                        zoom[analysisMode] *= 1.05;
                        wip[analysisMode] *= zoom[analysisMode];
                        break;
                    case 18:
                        viewChanged = 1;
                        wip[analysisMode] /= zoom[analysisMode];
                        zoom[analysisMode] /= 1.05;
                        wip[analysisMode] *= zoom[analysisMode];
                        break;
                    case 16:
                        viewChanged = 1;
                        wip[analysisMode] += 10;
                        break;
                    case 4:
                        viewChanged = 1;
                        wip[analysisMode] -= 10;
                        break;
                    case 17:
                        viewChanged = 1;
                        magnify[analysisMode] *= 1.1;
                        break;
                    case 5:
                        viewChanged = 1;
                        magnify[analysisMode] /= 1.1;
                        break;
                    case 2:
                        windowMode ^= 1;
                        graphChanged = 1;
                        break;
                    case 25:
                        analysisMode = (analysisMode+1)%analyses;
                        graphChanged = 1;
                        break;
                    case 19:
                        N = std::min(1<<18, N*2);
                        sizeChanged = 1;
                        break;
                    case 6:
                        N = std::max(4, N/2);
                        sizeChanged = 1;
                        break;
                    default:
                        std::cout << event.key.code << '\n';
                }

            } else if(event.type == sf::Event::MouseMoved){
                indicatorChanged = 1;
                indpos = event.mouseMove.x;
            }
		}

        if(sizeChanged){
            graph = sf::VertexArray(sf::LineStrip, N);
            for(int i=0; i<N; i++) graph[i].color = graphColor;
            graphChanged = 1;
        }

        if(graphChanged){
            
            std::vector<float > tmp(N);
            
            if(windowMode){
                for(int i=0; i<N; i++) tmp[i] = waves[wpos+i]*(0.5f-std::cos(2*PI*i/N)*0.5f);
            } else {
                for(int i=0; i<N; i++) tmp[i] = waves[wpos+i];
            }

            switch(analysisMode){
                case 0:
                    // no analysis
                    spectrum.resize(N);
                    for(int i=0; i<N; i++) spectrum[i] = {tmp[i], 0};
                    break;
                case 1:
                    // spectral analysis
                    spectrum = myFFT.dft(tmp);
                    break;
                case 2:
                    // autocorrelation
                    tmp = myFFT.convolution(tmp, tmp, 2*N-1, 0, 1);
                    spectrum.resize(N);
                    for(int i=0; i<N; i++) spectrum[i] = {tmp[N-1+i]*(1+i)/N, 0};

                    float maxs = -1e9;

                    for(int i=1; i<N; i++){
                        if(fileFrequency/i < maxFundamental && tmp[N-1+i] > maxs){
                            maxs = tmp[N-1+i];
                            fundamental = fileFrequency/i;
                        }
                    }

                    break;
            }

            viewChanged = 1;
        }

        if(viewChanged){
            
            switch(analysisMode){
                case 0:
                    // no analysis
                    for(int i=0; i<N; i++){
                        graph[i].position = sf::Vector2f(graphLeft+i*zoom[analysisMode]
                                +wip[analysisMode],
                                (float)graphDown-graphHeight/2
                                -spectrum[i].real()
                                *graphHeight*magnify[analysisMode]/2);
                    }
                    break;
                case 1:
                    // spectral analysis
                    for(int i=0; i<N; i++){
                        graph[i].position = sf::Vector2f(graphLeft+i*zoom[analysisMode]
                                +wip[analysisMode],
                                (float)graphDown-(std::abs(spectrum[i])/N)
                                *graphHeight*magnify[analysisMode]);
                    }
                    break;
                case 2:
                    // autocorrelation
                    for(int i=0; i<N; i++){
                        graph[i].position = sf::Vector2f(graphLeft+i*zoom[analysisMode]
                                +wip[analysisMode],
                                (float)graphDown-graphHeight/2
                                -spectrum[i].real()
                                *graphHeight*magnify[analysisMode]/(2*N));
                    }
                    break;
            }

            indicatorChanged = 1;
        }

        if(indicatorChanged){ 
            
            ipos = std::max(0, std::min(N-1,
                      (int)std::round(((indpos-graphLeft-wip[analysisMode])/zoom[analysisMode]))));
            
            indicatorNeedle[0].position = sf::Vector2f(indicatorSize, indicatorSize);
            indicatorNeedle[1].position = sf::Vector2f(
                    indicatorSize*(1+std::polar(1.0f, std::arg(spectrum[ipos])).real()),
                    indicatorSize*(1+std::polar(1.0f, std::arg(spectrum[ipos])).imag()));

            indicatorLine[0].position = sf::Vector2f(indpos, graphDown);
            indicatorLine[1].position = sf::Vector2f(indpos, 50);
             
            switch(analysisMode){
                case 0:
                    // no analysis
                    frequencyIndicator.setString(
                    "Time: "+std::to_string((wpos+ipos)/fileFrequency)+" s\n"
                    +"Amplitude: "+std::to_string(spectrum[ipos].real()));
                    break;
                case 1:
                    // spectral analysis
                    frequencyIndicator.setString(
                    "Frequency: "+std::to_string((int)std::round(fileFrequency*ipos/N))+" Hz\n"
                    +"Amplitude: "+std::to_string(std::abs(spectrum[ipos]))+"\n"
                    +"Phase: "+std::to_string(std::arg(spectrum[ipos]))+" rad");
                    break;
                case 2:
                    // autocorrelation
                    frequencyIndicator.setString(
                    "Time shift: "+std::to_string(ipos/fileFrequency)+" s\n"
                    +"Frequency: "+std::to_string(fileFrequency/ipos)+" Hz\n"
                    +"Fundamental: "+std::to_string(fundamental)+ " Hz\n"
                    +"Amplitude: "+std::to_string(spectrum[ipos].real()));
                    break;
            }

            redraw = 1;

        }
    	
        if(redraw){
            window.clear();
            window.draw(background);
            window.draw(phaseIndicator);
            window.draw(graph);
            window.draw(indicatorLine);
            window.draw(frequencyIndicator);
            window.draw(indicatorNeedle);
            window.display();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        sizeChanged = 0;
        graphChanged = 0;
        viewChanged = 0;
        indicatorChanged = 0;
        redraw = 0;

    }

    return 0;
}
