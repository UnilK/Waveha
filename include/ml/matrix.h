#pragma once

#include "ml/layer.h"

namespace ml {

class Matrix : public Layer {

public:

    Matrix(
            std::vector<float> &source,
            std::vector<float> &destination);

    void push();
    void pull();
    void change(double factor);
    
    void save(app::Saver &saver);
    void load(app::Loader &loader);

    std::string get_type();

private:

    std::vector<std::vector<float> > matrix, changes;

};

}

