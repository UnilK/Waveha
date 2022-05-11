#include "ml/factory.h"
#include "ml/field.h"
#include "ml/matrix.h"
#include "ml/deloc.h"
#include "ml/roll.h"
#include "ml/ft.h"
#include "ml/judge.h"
#include "ml/norm.h"
#include "ml/reblock.h"

#include <assert.h>

namespace ml {

namespace Factory {

std::string matrix = "matrix";
std::string cmatrix = "cmatrix";
std::string unroll = "unroll";
std::string reroll = "reroll";
std::string deloc = "deloc";
std::string ft = "ft";
std::string norm = "norm";
std::string cnorm = "cnorm";
std::string reblock = "reblock";

std::string a1 = "a1";
std::string v1 = "v1";
std::string vv1 = "vv1";
std::string s1 = "s1";
std::string e11 = "e11";
std::string csig = "csig";
std::string v2 = "v2";
std::string s2 = "s2";
std::string abs1 = "abs1";
std::string abs2 = "abs2";
std::string up2 = "up2";
std::string relu = "relu";

std::string average = "average";
std::string phaselax = "phaselax";

}

Layer *create_layer(std::string type, arrays left, arrays right, args a){
    
    if(type == Factory::matrix && Matrix::ok(left, right, a))
        return new Matrix(left, right, a);
    
    if(type == Factory::cmatrix && CMatrix::ok(left, right, a))
        return new CMatrix(left, right, a);
    
    if(type == Factory::unroll && Unroll::ok(left, right, a))
        return new Unroll(left, right, a);
    
    if(type == Factory::reroll && Reroll::ok(left, right, a))
        return new Reroll(left, right, a);
    
    if(type == Factory::deloc && Deloc::ok(left, right, a))
        return new Deloc(left, right, a);
    
    if(type == Factory::ft && FT::ok(left, right, a))
        return new FT(left, right, a);
   
    if(type == Factory::norm && Norm::ok(left, right, a))
        return new Norm(left, right, a);
   
    if(type == Factory::cnorm && CNorm::ok(left, right, a))
        return new CNorm(left, right, a);

    if(type == Factory::reblock && Reblock::ok(left, right, a))
        return new Reblock(left, right, a);


    if(type == Factory::v1 && V1Field::ok(left, right, a))
        return new V1Field(left, right, a);
    
    if(type == Factory::vv1 && VV1Field::ok(left, right, a))
        return new VV1Field(left, right, a);
    
    if(type == Factory::a1 && A1Field::ok(left, right, a))
        return new A1Field(left, right, a);
    
    if(type == Factory::s1 && S1Field::ok(left, right, a))
        return new S1Field(left, right, a);
    
    if(type == Factory::e11 && E11Field::ok(left, right, a))
        return new E11Field(left, right, a);
    
    if(type == Factory::csig && CsigField::ok(left, right, a))
        return new CsigField(left, right, a);
    
    if(type == Factory::v2 && V2Field::ok(left, right, a))
        return new V2Field(left, right, a);
    
    if(type == Factory::s2 && S2Field::ok(left, right, a))
        return new S2Field(left, right, a);
    
    if(type == Factory::abs1 && Abs1Field::ok(left, right, a))
        return new Abs1Field(left, right, a);
    
    if(type == Factory::abs2 && Abs2Field::ok(left, right, a))
        return new Abs2Field(left, right, a);
    
    if(type == Factory::up2 && Up2Field::ok(left, right, a))
        return new Up2Field(left, right, a);
    
    if(type == Factory::relu && ReluField::ok(left, right, a))
        return new ReluField(left, right, a);
    
    return nullptr;
}

Judge *create_judge(std::string type, std::vector<float> &result){

    if(type == Factory::average && Average::ok(result))
        return new Average(result);
    
    if(type == Factory::phaselax && Phaselax::ok(result))
        return new Phaselax(result);
   
    return nullptr;
}

}

