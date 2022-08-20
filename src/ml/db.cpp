#include "ml/db.h"
#include "ml/util.h"

namespace ml {

TrainingData::~TrainingData(){}

InputLabel TrainingData::get_random(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    if(size() == 0) return {{}, {}};
    return get(rng32()%size());
}

InputLabel TrainingData::get(size_t position){
    if(cached && size() != 0) return cache[position % size()];
    return direct_get(position);
}

InputLabel TrainingData::direct_get(size_t position){ return {{}, {}}; }

size_t TrainingData::size() const { return 0; }

void TrainingData::switch_to_cache(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    if(cached) clear_cache();
    
    for(size_t i=0; i<size(); i++) cache.push_back(get(i));
    
    cached = 1;
}

void TrainingData::clear_cache(){
    
    std::lock_guard<std::recursive_mutex> lock(mutex);
    
    cached = 0;
    cache.clear();
}

}

