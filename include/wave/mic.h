#pragma once

#include "wave/buffer.h"

#include <SFML/Audio/SoundRecorder.hpp>

namespace wave{

class Mic : public Buffer, public sf::SoundRecorder {

public:

    Mic();

    bool onStart();

    bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount);

    void onStop();

};

}
