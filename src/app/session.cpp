#include "app/session.h"

#include <string>
#include <string.h>

namespace app {

// session ////////////////////////////////////////////////////////////////////

Session::Session(App *a) : app(a) {}

void Session::save(){

}

void Session::load(std::string file){

}

void Session::rename(std::string name_){
    name = name_;
}

void Session::create_new(std::string name_){
    name = name_;
}

// presistent /////////////////////////////////////////////////////////////////

void Presistent::save(Saver&){}

void Presistent::load(Loader&){}

// saver //////////////////////////////////////////////////////////////////////

Saver::Saver(std::string file) : out(file) {}

void Saver::write_string(std::string s){
    out.write(s.data(), s.size());
    out.put('\0');
}

void Saver::write_int(int i){
    out.write((char*)&i, sizeof(int));
}

void Saver::write_float(float f){
    out.write((char*)&f, sizeof(float));
}

void Saver::write_block(int bytes, void *data){
    write_int(bytes);
    out.write((char*)data, bytes);
}

void Saver::close(){
    out.close();
}

bool Saver::bad(){
    return out.bad();
}

// loader /////////////////////////////////////////////////////////////////////

Loader::Loader(std::string file) : in(file) {}

std::string Loader::read_string(){
    std::string str;
    std::getline(in, str, '\0');
    return str;
}

int Loader::read_int(){
    int i;
    in.read((char*)&i, sizeof(int));
    return i;
}

float Loader::read_float(){
    float f;
    in.read((char*)&f, sizeof(float));
    return f;
}

std::vector<char> Loader::read_block(){
    int size = read_int();
    std::vector<char> data(size);
    in.read(data.data(), size);
    return data;
}

void Loader::close(){
    in.close();
}

bool Loader::bad(){
    return in.bad();
}

}

