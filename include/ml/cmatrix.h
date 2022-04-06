#pragma once

#include "ml/layer.h"

#include <complex>

namespace ml {

class CMatrix : public Layer {

public:

    CMatrix(
            std::vector<float> &source,
            std::vector<float> &destination);

    void push();
    void pull();
    void change(double factor);
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    static bool ok(std::vector<float> &left, std::vector<float> &right);
    
    std::string get_type();

private:
    
    std::vector<std::complex<float> > cleft, cright;
    std::vector<std::vector<std::complex<float> > > matrix, changes;

};

}

