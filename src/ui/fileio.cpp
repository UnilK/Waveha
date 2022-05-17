#include "ui/fileio.h"

#include <cassert>

namespace ui {

// saver //////////////////////////////////////////////////////////////////////

Saver::Saver(std::string file) : out(file) {}

void Saver::write_byte(char c){
    out.write(&c, 1);
}

void Saver::write_string(std::string s){
    for(unsigned i=0; i<s.size(); i++) if(s[i] == '\0') s = s.substr(0, i);
    out.write(s.data(), s.size());
    out.put('\0');
}

void Saver::write_bool(bool b){
    out.write((char*)&b, sizeof(bool));
}

void Saver::write_int(int i){
    out.write((char*)&i, sizeof(int));
}

void Saver::write_unsigned(unsigned u){
    out.write((char*)&u, sizeof(unsigned));
}

void Saver::write_float(float f){
    out.write((char*)&f, sizeof(float));
}

void Saver::write_double(double d){
    out.write((char*)&d, sizeof(double));
}

void Saver::write_complex(std::complex<float> c){
    write_float(c.real());
    write_float(c.imag());
}

void Saver::write_block(unsigned bytes, void *data){
    write_unsigned(bytes);
    out.write((char*)data, bytes);
}

void Saver::close(){
    out.close();
}

bool Saver::bad(){
    return !out.good();
}

void Saver::open_chunk(std::string name){
    write_string(name);
    chunks.push_back(out.tellp());
    write_unsigned(0);
}

void Saver::close_chunk(){
    assert(chunks.size() > 0);
    unsigned position = out.tellp();
    unsigned length = position - chunks.back() - sizeof(unsigned);
    out.seekp(chunks.back());
    write_unsigned(length);
    chunks.pop_back();
    out.seekp(position);
}

// loader /////////////////////////////////////////////////////////////////////

Loader::Loader(std::string file) : in(file) {}

std::string Loader::read_string(){
    std::string str;
    std::getline(in, str, '\0');
    return str;
}

char Loader::read_byte(){
    char c;
    in.read((char*)&c, 1);
    return c;
}

bool Loader::read_bool(){
    bool b;
    in.read((char*)&b, sizeof(bool));
    return b;
}

int Loader::read_int(){
    int i;
    in.read((char*)&i, sizeof(int));
    return i;
}

unsigned Loader::read_unsigned(){
    unsigned u;
    in.read((char*)&u, sizeof(unsigned));
    return u;
}

float Loader::read_float(){
    float f;
    in.read((char*)&f, sizeof(float));
    return f;
}

double Loader::read_double(){
    double d;
    in.read((char*)&d, sizeof(double));
    return d;
}

std::complex<float> Loader::read_complex(){
    return {read_float(), read_float()};
}

std::vector<char> Loader::read_raw(unsigned size){
    std::vector<char> data(size);
    in.read(data.data(), size);
    return data;
}

std::vector<char> Loader::read_block(){
    return read_raw(read_unsigned());
}

std::string Loader::probe_chunk(){
    unsigned position = in.tellg();
    std::string name = read_string();
    in.seekg(position);
    return name;
}

bool Loader::enter_chunk(std::string name){
    
    unsigned position = in.tellg();
    std::string cname = read_string();
    read_unsigned();

    if(cname == name) return 1;
    
    in.seekg(position);
    return 0;
}

void Loader::skip_chunk(){
    read_string();
    read_block();
}

void Loader::close(){
    in.close();
}

bool Loader::bad(){
    return !in.good();
}

}

