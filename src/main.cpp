#include "app/app.h"

#include <iostream>

int main(int argc, char *argv[]){

    srand(time(0));

    ui::Style::load("res/styles/default.style");

    app::App waveha;
    waveha.loop();

    return 0;
}
