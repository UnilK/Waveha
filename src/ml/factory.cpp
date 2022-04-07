#include "ml/factory.h"
#include "ml/field.h"
#include "ml/matrix.h"
#include "ml/cmatrix.h"
#include "ml/deloc.h"

#include <assert.h>

namespace ml {

namespace Factory {

std::string matrix = "matrix";
std::string cmatrix = "cmatrix";
std::string a1 = "a1";
std::string v1 = "v1";
std::string s1 = "s1";
std::string e11 = "e11";
std::string v2 = "v2";
std::string average1 = "average1";
std::string average2 = "average2";
std::string abs1 = "abs1";
std::string abs2 = "abs2";
std::string up2 = "up2";
std::string deloc = "deloc";
std::string relu = "relu";

}

Layer *create_layer(
        std::string type, 
        std::vector<float> &left,
        std::vector<float> &right){
    
    if(type == Factory::matrix && Matrix::ok(left, right))
        return new Matrix(left, right);
    if(type == Factory::cmatrix && CMatrix::ok(left, right))
        return new CMatrix(left, right);
    if(type == Factory::v1 && V1Field::ok(left, right))
        return new A1Field(left, right);
    if(type == Factory::a1 && A1Field::ok(left, right))
        return new V1Field(left, right);
    if(type == Factory::s1 && S1Field::ok(left, right))
        return new S1Field(left, right);
    if(type == Factory::e11 && E11Field::ok(left, right))
        return new E11Field(left, right);
    if(type == Factory::v2 && V2Field::ok(left, right))
        return new V2Field(left, right);
    if(type == Factory::average1 && Average1Field::ok(left, right))
        return new Average1Field(left, right);
    if(type == Factory::average2 && Average2Field::ok(left, right))
        return new Average2Field(left, right);
    if(type == Factory::abs1 && Abs1Field::ok(left, right))
        return new Abs1Field(left, right);
    if(type == Factory::abs2 && Abs2Field::ok(left, right))
        return new Abs2Field(left, right);
    if(type == Factory::up2 && Up2Field::ok(left, right))
        return new Up2Field(left, right);
    if(type == Factory::deloc && Deloc::ok(left, right))
        return new Deloc(left, right);
    if(type == Factory::relu && ReluField::ok(left, right))
        return new ReluField(left, right);
    
    return nullptr;
}

}

