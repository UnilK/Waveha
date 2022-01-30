#pragma once

#include "wave/buffer.h"

#include <SFML/Audio/SoundRecorder.hpp>

namespace wave {

class Mic : public sf::SoundRecorder, public Buffer {

public:

    Mic();

    virtual ~Mic();

    bool onStart();

    bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount);

    void onStop();

};

}
