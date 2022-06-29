#include "ml/db.h"

namespace ml {

TrainingData::~TrainingData(){}

InputLabel TrainingData::get_random(){ return {{}, {}}; }

InputLabel TrainingData::get_next(){ return {{}, {}}; }

void TrainingData::go_to(size_t position){}

size_t TrainingData::get_size(){ return 0; }

}

