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

#include "math/constants.h"
#include "wavestream.h"
#include "math/FFT.h"
#include "math/sincFIR.h"
#include "wave/voiceTransform.h"
#include "wave/vocal.h"
#include "wave/vocalTransform.h"
#include "math/FT.h"

int main(){

	iwavestream I;
    FastSincFIR myFIR(30, 1<<18);

	std::string file;
	std::cin >> file;

	while(!I.open(file)){
        std::cout << "file not found\n";
        std::cin >> file;
    }

    std::vector<float> waves;
    I.read_file(waves);
    I.close();

	int N = 1<<10;
   
    float fileFrequency = I.get_frame_rate();
    float frameHeight = 800, frameWidth = 1400;
    float indicatorSize = 100, graphHeight = frameHeight - 100;

    VoiceTransform voitra((int32_t)fileFrequency, N);

    sf::RenderWindow window(sf::VideoMode(frameWidth, frameHeight), "wave analyzer");

    sf::RectangleShape background({frameWidth, frameHeight});
    background.setFillColor(sf::Color(0x55, 0x55, 0x56));

    sf::CircleShape phaseIndicator(indicatorSize);
    phaseIndicator.setFillColor(sf::Color(0x66, 0x66, 0x67));

    sf::VertexArray graph(sf::LineStrip, N);
    sf::VertexArray indicatorNeedle(sf::LineStrip, 2);
    sf::VertexArray indicatorLine(sf::LineStrip, 2);
    sf::VertexArray intervalLine(sf::LineStrip, 2);
    std::vector<sf::VertexArray> intervals;
    
    sf::Color graphColor(0xff, 0xaa, 0xaa);
    sf::Color needleColor(0xaa, 0xff, 0xaa);
    sf::Color lineColor(0xaa, 0xaa, 0xff);
    sf::Color intervalColor(0x88, 0x77, 0x55);

    for(int i=0; i<N; i++) graph[i].color = graphColor;
    for(int i=0; i<2; i++) indicatorNeedle[i].color = needleColor;
    for(int i=0; i<2; i++) indicatorLine[i].color = lineColor;
    for(int i=0; i<2; i++) intervalLine[i].color = intervalColor;
    
    sf::Font font;
    font.loadFromFile("./res/fonts/UMR.ttf");
    sf::Text frequencyIndicator("Frequancy: NaN", font);
    
    frequencyIndicator.setCharacterSize(20);
    frequencyIndicator.setFillColor(needleColor);
    frequencyIndicator.setPosition(20.0f, 20.0f+indicatorSize*2);

    int wpos = 0, ipos = 0, indpos = 0;

    float graphLeft = indicatorSize*2 + 50;
    float graphDown = graphHeight + 50;

    std::vector<std::complex<float> > spectrum(N, {0, 0});

    bool sizeChanged = 0, graphChanged = 1, viewChanged = 1, indicatorChanged = 1, redraw = 1;
    
    int windowMode = 0;
    int analyses = 2, analysisMode = 0;

    float magnify[2] = {1, 10};
    float zoom[2] = {1, 1};
    float wip[2] = {0, 0};

    float fundamental = 0;

    bool purewave = 0;
    float phase = 0, freq = 100;
    bool scrollSpeed = 1;
    
    float freqLow = 40, freqHigh = 700, powa = 1.0f;
    float interval = 120;

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
                        if(wpos+N < (int)waves.size()){
                            N = std::min(1<<18, N*2);
                            sizeChanged = 1;
                        }
                        break;
                    case 6:
                        N = std::max(4, N/2);
                        sizeChanged = 1;
                        break;
                    case 21:
                        graphChanged = 1;
                        wpos = std::max(0, wpos-10);
                        break;
                    case 1:
                        graphChanged = 1;
                        wpos = std::min((int)waves.size()-N-1, wpos+10);
                        break;
                    case 15:
                        purewave ^= 1;
                        graphChanged = 1;
                        break;
                    case 8:
                        if(scrollSpeed) phase += 0.1;
                        else phase += 0.01;
                        graphChanged = 1;
                        std::cout << phase << "\n";
                        break;
                    case 10:
                        if(scrollSpeed) phase -= 0.1;
                        else phase -= 0.01;
                        graphChanged = 1;
                        std::cout << phase << "\n";
                        break;
                    case 14:
                        if(scrollSpeed) freq += 20;
                        else freq += 0.1;
                        graphChanged = 1;
                        std::cout << freq << "\n";
                        break;
                    case 11:
                        if(scrollSpeed) freq -= 20;
                        else freq -= 0.1;
                        graphChanged = 1;
                        std::cout << freq << "\n";
                        break;
                    case 20:
                        scrollSpeed ^= 1;
                        break;
                    case 9:
                        std::cin >> freqLow >> freqHigh >> powa;
                        graphChanged = 1;
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
            graph.resize(N);
            for(int i=0; i<N; i++) graph[i].color = graphColor;
            graphChanged = 1;
        }

        if(graphChanged){
            
            std::vector<float > tmp(N);
           
            if(purewave){
                float coeff = 2*PIF*freq/fileFrequency;
                for(int i=0; i<N; i++){
                    tmp[i] = std::cos(phase+coeff*i);
                }
            } else {
                if(windowMode){
                    for(int i=0; i<N; i++) tmp[i] = waves[wpos+i]*(0.5f-std::cos(2*PIF*i/N)*0.5f);
                } else {
                    for(int i=0; i<N; i++) tmp[i] = waves[wpos+i];
                }
            }
                    
            float minf = 80, maxf = 800, maxAmp = 0;
            int buffe = 20;

            switch(analysisMode){
                case 0:
                    // no analysis
                    spectrum.resize(N);
                    for(int i=0; i<N; i++) spectrum[i] = {tmp[i], 0};
                    break;
                case 1:
                    // spectral analysis
                    spectrum = math::fft(tmp);
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
                                *graphHeight*magnify[analysisMode]/2);
                    }
                    break;
                case 3:
                    // peak match
                    for(int i=0; i<N; i++){
                        graph[i].position = sf::Vector2f(graphLeft+i*zoom[analysisMode]
                                +wip[analysisMode],
                                (float)graphDown-(std::abs(spectrum[i])/N)
                                *graphHeight*magnify[analysisMode]);
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
                    indicatorSize*(1-std::polar(1.0f, std::arg(spectrum[ipos])).imag()));

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

                    for(int i=0; interval*i<fileFrequency; i++){
                        intervals.push_back(intervalLine);
                        float pos = graphLeft+interval*i*N*zoom[analysisMode]/fileFrequency
                                +wip[analysisMode];
                        intervals[i][0].position = sf::Vector2f(pos, graphDown);
                        intervals[i][1].position = sf::Vector2f(pos, 50);
                    }

                    break;
                case 2:
                    // autocorrelation
                    frequencyIndicator.setString(
                    +"Frequency: "+std::to_string(fileFrequency/ipos)+" Hz\n"
                    +"Fundamental: "+std::to_string(fundamental)+ " Hz\n"
                    +"Amplitude: "+std::to_string(spectrum[ipos].real()));
                    break;
                case 3:
                    // peak match
                    frequencyIndicator.setString(
                    +"Frequency: "+std::to_string(freqLow+(freqHigh-freqLow)/N*ipos)+" Hz\n"
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
            for(auto &i : intervals) window.draw(i);
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
        intervals.clear();

    }

    return 0;
}
