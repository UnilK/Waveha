#pragma once

#include "ml/layer.h"

namespace ml {

class Matrix : public Layer {

public:

    Matrix(arrays in, arrays out, args);

    void push();
    void pull();
    void change(double factor);
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    static bool ok(arrays in, arrays out, args);

    std::string get_type();

protected:

    array &l, &r;
    std::vector<std::vector<float> > matrix, changes;

};



class CMatrix : public Layer {

public:

    CMatrix(arrays in, arrays out, args);

    virtual void push();
    virtual void pull();
    void change(double factor);
    
    void save(ui::Saver &saver);
    void load(ui::Loader &loader);

    static bool ok(arrays in, arrays out, args);
    
    virtual std::string get_type();

protected:
    
    array &l, &r;
    std::vector<std::vector<std::complex<float> > > matrix, changes;

};

}

