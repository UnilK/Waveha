#pragma once

#include "wave/source.h"

#include "wstream/wstream.h"

namespace wave {

class File : public Source {

    // class for reading audio straight from a file

public:

    File();
    File(std::string fileName);

    void open(std::string fileName);

    void seek(uint32_t sample);
    std::vector<float> pull(uint32_t amount);
    std::vector<float> get(uint32_t amount, uint32_t begin);

protected:

    iwstream file;

};

}
