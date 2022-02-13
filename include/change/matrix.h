#pragma once

#include <vector>
#include <complex>

namespace change {

class Matrix {

    // square matrix for probing the frequency
    // decomposition of a sound.

public:

    Matrix(unsigned size);

    void resize(unsigned size);
    unsigned size() const;

    std::complex<float> operator()(unsigned r, unsigned c) const;
    std::vector<std::complex<float> > operator*=(const std::vector<std::complex<float> > &vec) const;

    void set(unsigned r, unsigned c, std::complex<float> m);
    void add(unsigned r, unsigned c, std::complex<float> m);
    void multiply(unsigned r, unsigned c, std::complex<float> m);

    void set_unit();
    void set_nil();
    void phase_shuffle();

private:

    std::vector<std::vector<std::complex<float> > > x;

};

}

