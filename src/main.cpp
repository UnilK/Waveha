#include "app/app.h"

#include <iostream>

using namespace std;

int main(int argc, char *argv[]){

    ui::Style::load("res/styles/default.style");

    app::App waveha;
    waveha.loop();

    return 0;
}
