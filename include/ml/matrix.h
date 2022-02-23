#pragma once

#include "ml/layer.h"

namespace ml {

class Matrix : public Layer {

public:

    Matrix(
            std::vector<std::complex<float> > &source,
            std::vector<std::complex<float> > &destination);

    void push();
    void pull();
    void change(double factor);
    
    void save(app::Saver &saver);
    void load(app::Loader &loader);

    std::string get_type();

    static const std::string type;

private:

    std::vector<std::vector<std::complex<float> > > matrix, changes;

};

}

