#include "ml/factory.h"
#include "ml/field.h"
#include "ml/matrix.h"

#include <assert.h>

namespace ml {

Layer *create_layer(
        std::string type, 
        std::vector<std::complex<float> > &left,
        std::vector<std::complex<float> > &right){
    
    if(type == Field::type) return new Field(left, right);
    if(type == Matrix::type) return new Matrix(left, right);
    return nullptr;
}

}

