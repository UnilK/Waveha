#pragma once

namespace designi {

class Binder {

public:

    Binder();

    float process(float sample);

    void set_bound(float);

    int get_delay();

private:

    float bound;

};

}

