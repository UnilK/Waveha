#include "app/app.h"

#include <iostream>

int main(int argc, char *argv[]){

    ui::Style::load("res/styles/default.style");

    app::App waveha;
    waveha.loop();

    return 0;
}
