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
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    std::string get_type();

private:

    std::vector<std::vector<float> > matrix, changes;

};

}

