#include "app/session.h"
#include "app/app.h"

#include <string>
#include <string.h>

namespace app {

// session ////////////////////////////////////////////////////////////////////

Session::Session(App *a) :
    dir(*this),
    app(*a)
{}

void Session::save(){
    
    Saver saver("sessions/" + name + ".was");

    if(saver.bad()) return;

    app.audio.save(saver);
    app.layout.save(saver);

    saver.close();
}

void Session::load(std::string file){

    Loader loader("sessions/" + file + ".was");

    if(loader.bad()) return;

    app.audio.load(loader);
    app.layout.load(loader);

    name = file;

    app.layout.set_label(name);

    loader.close();
}

void Session::rename(std::string name_){
    name = name_;
    app.layout.set_label(name);
}

void Session::create_new(std::string name_){
    name = name_;
    app.layout.reset();
    app.audio.reset();
    app.layout.set_label(name);
}

std::string Session::get_name(){ return name; }

bool Session::valid_file(std::string file){
    
    std::ifstream I("sessions/" + file + ".was");

    return I.good();
}

// directory //////////////////////////////////////////////////////////////////

SessionDir::SessionDir(Session &s) :
    session(s)
{
    put_function("save", [&](ui::Command c){ save(c); });
    put_function("load", [&](ui::Command c){ load(c); });
    put_function("name", [&](ui::Command c){ rename(c); });
    put_function("new", [&](ui::Command c){ create_new(c); });

    document("save", "{name} save current session");
    document("load", "[name] load a session from a file and discard the current one");
    document("name", "[new name] rename the current session");
    document("new", "[name] start a new session and discard the current one");
}

void SessionDir::save(ui::Command c){
    if(!c.empty()) rename(c);
    session.save();
}

void SessionDir::load(ui::Command c){
    
    std::string file = c.pop();

    if(session.valid_file(file)){
        session.load(file);
    }
    else {
        c.source.push_error("invalid source file: \"" + file + "\"");
    }
}

void SessionDir::rename(ui::Command c){
    
    std::string name = c.pop();

    if(!name.empty()){
        session.rename(name);
    }
    else {
        c.source.push_error("name must not be empty");
    }
}

void SessionDir::create_new(ui::Command c){
    
    std::string name = c.pop();

    if(!name.empty()){
        session.create_new(name);
    }
    else {
        c.source.push_error("new name must not be empty");
    }
}

// presistent /////////////////////////////////////////////////////////////////

void Presistent::save(Saver&){}

void Presistent::load(Loader&){}

// saver //////////////////////////////////////////////////////////////////////

Saver::Saver(std::string file) : out(file) {}

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
    return out.bad();
}

// loader /////////////////////////////////////////////////////////////////////

Loader::Loader(std::string file) : in(file) {}

std::string Loader::read_string(){
    std::string str;
    std::getline(in, str, '\0');
    return str;
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

std::vector<char> Loader::read_block(){
    unsigned size = read_unsigned();
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

