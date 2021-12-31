#pragma once

#include "app/box.h"
#include "app/graph.h"

class Analyzer : public Box {

public:
    
    Analyzer(ui::Frame *parent_, std::string title_, kwargs values = {});

protected:

   ui::Frame inner;
   Graph graph;

};

