#pragma once

#include "wave/source.h"

#include "wstream/wstream.h"

namespace wave {

class File : public Source {

    // class for reading audio straight from a file

public:

    File();
    File(std::string fileName);
    virtual ~File();

    void open(std::string fileName);
    unsigned tell();
    unsigned size();

    void seek(unsigned sample);
    unsigned pull(unsigned amount, std::vector<float> &samples);
    std::vector<float> get(unsigned amount, unsigned begin);

protected:

    iwstream file;

};

}

