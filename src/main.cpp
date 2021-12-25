#include "app/app.h"
#include "wave/audioFile.h"

#include <SFML/Audio.hpp>

#include <iostream>

using namespace std;

int main(int argc, char *argv[]){

    string name;
    cin >> name;

    wave::AudioFile file;

    while(!file.open("audio/" + name)){
        cout << "audio/"+name << " epic fail\n";
        cin >> name;
    }

    file.refill();
    file.readMode = 1;
    file.play();

    while(file.getStatus() == wave::ReadableAudio::Playing){
        if(file.good()){
            file.refill();
        }
        sf::sleep(sf::seconds(0.1f));
    }

    return 0;
}
