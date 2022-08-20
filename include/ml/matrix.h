#pragma once

#include "ml/layer.h"

namespace ml {

class Matrix : public Layer {

    // runs a vector through a matrix

public:

    Matrix(arrays in, arrays out, args);

    void push();
    void pull();
    
    std::pair<float, unsigned> sum_change();
    void change(float factor, float decay);
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    static bool ok(arrays in, arrays out, args);

    std::string get_type();

protected:

    array &l, &r;
    std::vector<std::vector<float> > matrix, changes;

};



class CMatrix : public Layer {

    // runs a complex valued vector through a compled valued matrix

public:

    CMatrix(arrays in, arrays out, args);

    virtual void push();
    virtual void pull();
    
    std::pair<float, unsigned> sum_change();
    void change(float factor, float decay);
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    static bool ok(arrays in, arrays out, args);
    
    virtual std::string get_type();

protected:
    
    array &l, &r;
    std::vector<std::vector<std::complex<float> > > matrix, changes;

};



class PMatrix : public Layer {

    // runs a complex valued vector through a compled valued matrix

public:

    PMatrix(arrays in, arrays out, args);

    virtual void push();
    virtual void pull();
    
    static bool ok(arrays in, arrays out, args);
    
    virtual std::string get_type();

protected:
    
    array &l, &r, &matrix;

};

}

