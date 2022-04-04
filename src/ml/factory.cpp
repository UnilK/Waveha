#include "ml/factory.h"
#include "ml/field.h"
#include "ml/matrix.h"

#include <assert.h>

namespace ml {

namespace Factory {

std::string matrix = "matrix";
std::string v1 = "v1";
std::string v2 = "v2";
std::string average1 = "average1";
std::string average2 = "average2";

}

Layer *create_layer(
        std::string type, 
        std::vector<float> &left,
        std::vector<float> &right){
    
    if(type == Factory::matrix) return new Matrix(left, right);
    if(type == Factory::v1) return new V1Field(left, right);
    if(type == Factory::v2) return new V2Field(left, right);
    if(type == Factory::average1) return new Average1Field(left, right);
    if(type == Factory::average2) return new Average2Field(left, right);
    
    return nullptr;
}

}
