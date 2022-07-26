#include "ui/fileio.h"

#include <cassert>

namespace ui {

// saver //////////////////////////////////////////////////////////////////////

Saver::Saver(){}

Saver::Saver(std::string file) : out(file) {}

bool Saver::open(std::string file){
    out.open(file);
    return !bad();
}

bool Saver::seek(size_t pos){
    out.seekp(pos);
    return !bad();
}

size_t Saver::tell(){
    return out.tellp();
}

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

void Saver::write_raw(unsigned bytes, void *data){
    out.write((char*)data, bytes);
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

Loader::Loader(){}

Loader::Loader(std::string file) : in(file) {}

bool Loader::open(std::string file){
    in.open(file);
    return !bad();
}

bool Loader::seek(size_t pos){
    in.seekg(pos);
    return !bad();
}

size_t Loader::tell(){
    return in.tellg();
}

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

// editor /////////////////////////////////////////////////////////////////////

Editor::Editor(){}

Editor::Editor(std::string file_) : file(file_) {}

bool Editor::open(std::string file_){
    file.open(file_);
    return !bad();
}

bool Editor::seekp(size_t pos){
    file.seekp(pos);
    return !bad();
}

size_t Editor::tellp(){
    return file.tellp();
}

void Editor::write_byte(char c){
    file.write(&c, 1);
}

void Editor::write_string(std::string s){
    for(unsigned i=0; i<s.size(); i++) if(s[i] == '\0') s = s.substr(0, i);
    file.write(s.data(), s.size());
    file.put('\0');
}

void Editor::write_bool(bool b){
    file.write((char*)&b, sizeof(bool));
}

void Editor::write_int(int i){
    file.write((char*)&i, sizeof(int));
}

void Editor::write_unsigned(unsigned u){
    file.write((char*)&u, sizeof(unsigned));
}

void Editor::write_float(float f){
    file.write((char*)&f, sizeof(float));
}

void Editor::write_double(double d){
    file.write((char*)&d, sizeof(double));
}

void Editor::write_complex(std::complex<float> c){
    write_float(c.real());
    write_float(c.imag());
}

void Editor::write_raw(unsigned bytes, void *data){
    file.write((char*)data, bytes);
}



bool Editor::seekg(size_t pos){
    file.seekg(pos);
    return !bad();
}

size_t Editor::tellg(){
    return file.tellg();
}

std::string Editor::read_string(){
    std::string str;
    std::getline(file, str, '\0');
    return str;
}

char Editor::read_byte(){
    char c;
    file.read((char*)&c, 1);
    return c;
}

bool Editor::read_bool(){
    bool b;
    file.read((char*)&b, sizeof(bool));
    return b;
}

int Editor::read_int(){
    int i;
    file.read((char*)&i, sizeof(int));
    return i;
}

unsigned Editor::read_unsigned(){
    unsigned u;
    file.read((char*)&u, sizeof(unsigned));
    return u;
}

float Editor::read_float(){
    float f;
    file.read((char*)&f, sizeof(float));
    return f;
}

double Editor::read_double(){
    double d;
    file.read((char*)&d, sizeof(double));
    return d;
}

std::complex<float> Editor::read_complex(){
    return {read_float(), read_float()};
}

std::vector<char> Editor::read_raw(unsigned size){
    std::vector<char> data(size);
    file.read(data.data(), size);
    return data;
}

void Editor::close(){
    file.close();
}

bool Editor::bad(){
    return !file.good();
}

}

