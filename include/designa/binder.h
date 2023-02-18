#pragma once

namespace designa {

class Binder {

public:

    Binder();

    float process(float sample);

    void set_bound(float);

private:

    float bound;

};

}

