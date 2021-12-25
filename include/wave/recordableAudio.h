#pragma once

#include "wave/audioBuffer.h"

#include <SFML/Audio/SoundRecorder.hpp>

namespace wave{

class RecordableAudio : public AudioBuffer, public sf::SoundRecorder {

public:

    RecordableAudio();

    bool onStart();

    bool onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount);

    void onStop();

};

}
