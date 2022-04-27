#pragma once

#include <vector>
#include <string>

namespace ml {

class Judge {

public:

    Judge(std::vector<float> &result_);

    virtual ~Judge();

    virtual void score(const std::vector<float> &solution) = 0;
    virtual void feedback(const std::vector<float> &solution) = 0;

    virtual std::string get_type() = 0;

protected:
    
    std::vector<float> &result;

};

class Average1 : public Judge {

public:

    using Judge::Judge;
    
    void score(const std::vector<float> &solution);
    void feedback(const std::vector<float> &solution);
    
    std::string get_type();
    static bool ok(const std::vector<float> &result);
};

class Average2 : public Judge {

public:

    using Judge::Judge;
    
    void score(const std::vector<float> &solution);
    void feedback(const std::vector<float> &solution);
    
    std::string get_type();
    static bool ok(const std::vector<float> &result);
};


}

