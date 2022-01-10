#include "ui/clock.h"

#include <thread>

namespace ui{

using namespace std::chrono;

Clock::Clock(long double seconds) :
    tick_length(seconds),
    clock(high_resolution_clock::now())
{}

bool Clock::try_tick(){
    if(high_resolution_clock::now() >= clock + tick_length){
        clock += tick_length;
        if(high_resolution_clock::now() >= clock)
            clock = high_resolution_clock::now();
        return 1;
    }
    return 0;
}

void Clock::force_sync_tick(){
    clock = high_resolution_clock::now();
}

void Clock::force_async_tick(){
    clock += tick_length;
}

void Clock::join_tick(){
    duration<long double> length = clock + tick_length - high_resolution_clock::now();
    if(length.count() < 0.00000001l){
        force_sync_tick();
        return;
    }
    std::this_thread::sleep_for(length);
    force_sync_tick();
}

long double Clock::get_duration(){
    duration<long double> length = high_resolution_clock::now() - clock;
    return length.count();
}

long double Clock::duration_and_tick(){
    duration<long double> length = high_resolution_clock::now() - clock;
    force_sync_tick();
    return length.count();
}

}
