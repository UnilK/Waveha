#pragma once

#include <complex>
#include <fstream>
#include <vector>

namespace ui {

class Saver {

public:

    Saver(std::string file);

    void write_byte(char);
    void write_string(std::string);
    void write_bool(bool);
    void write_int(int);
    void write_unsigned(unsigned);
    void write_float(float);
    void write_double(double);
    void write_complex(std::complex<float>);
    void write_block(unsigned bytes, void *data);

    void open_chunk(std::string name);
    void close_chunk();

    void close();

    bool bad();

private:

    std::vector<unsigned> chunks;
    std::ofstream out;

};



class Loader {

public:

    Loader(std::string file);

    char read_byte();
    std::string read_string();
    bool read_bool();
    int read_int();
    unsigned read_unsigned();
    float read_float();
    double read_double();
    std::complex<float> read_complex();
    std::vector<char> read_raw(unsigned);
    std::vector<char> read_block();

    std::string probe_chunk();
    bool enter_chunk(std::string name);
    void skip_chunk();

    void close();

    bool bad();

private:

    std::ifstream in;

};

}

