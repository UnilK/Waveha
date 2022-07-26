#include "ml/db.h"
#include "ml/util.h"

namespace ml {

TrainingData::~TrainingData(){}

InputLabel TrainingData::get_random(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    if(size() == 0) return {{}, {}};
    return get(rng32()%size());
}

InputLabel TrainingData::get(size_t position){ return {{}, {}}; }

size_t TrainingData::size() const { return 0; }

}

