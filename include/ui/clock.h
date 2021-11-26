#pragma once

#include <chrono>

namespace ui{

using namespace std::chrono;

class Clock {

protected:

    duration<long double> tick_length;
    time_point<high_resolution_clock, duration<long double> > clock;
    
public:

    Clock(long double seconds);

    bool try_tick();
    void force_sync_tick();
    void force_async_tick();
    void join_tick();

    long double get_duration();
    long double duration_and_tick();

};

}

