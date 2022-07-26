#include "ml/reblock.h"

#include <iostream>

namespace ml {

// reblock ////////////////////////////////////////////////////////////////////

void Reblock::push(){

    for(unsigned i=0, j=0, l=0, r=0;; l++, r++){
        
        if(l == left[i].size){ l = 0; i++; }
        if(r == right[j].size){ r = 0; j++; }
        if(i == left.size()) break;

        right[j][r] = left[i][l];
    }
}

void Reblock::pull(){
    
    if(nopull) return;

    for(unsigned i=0, j=0, l=0, r=0;; l++, r++){
        
        if(l == left[i].size){ l = 0; i++; }
        if(r == right[j].size){ r = 0; j++; }
        if(i == left.size()) break;

        left[i][l] = right[j][r];
    }
}

namespace Factory { extern std::string reblock; }
std::string Reblock::get_type(){ return Factory::reblock; }

bool Reblock::ok(arrays in, arrays out, args a){
    
    if(in.size() == 0 || out.size() == 0) return 0;
    
    unsigned lsize = 0, rsize = 0;
    for(auto &i : in){
        if(i.size == 0) return 0;
        lsize += i.size;
    }
    for(auto &i : out){
        if(i.size == 0) return 0;
        rsize += i.size;
    }
    
    return lsize == rsize;
}

// interleave /////////////////////////////////////////////////////////////////

Interleave::Interleave(arrays in, arrays out, args a) : 
    Layer(in, out, a)
{

    std::vector<unsigned> nums;
    for(auto i : a){
        try {
            nums.push_back(std::stoul(i));
        } catch (const std::invalid_argument &e){}
    }

    lpos.resize(nums[0]);
    rpos.resize(nums[3]);
    middle.resize(nums[2]);

    lpos.back() = nums[2]-nums[1];
    rpos.back() = nums[2]-nums[4];

    for(unsigned i=0; i<nums[0]-1; i++){
        lpos[i] = std::round((float)(nums[2]-nums[1])*i/(nums[0]-1));
    }
    for(unsigned i=0; i<nums[3]-1; i++){
        rpos[i] = std::round((float)(nums[2]-nums[4])*i/(nums[3]-1));
    }
    
    lcnt.resize(nums[2], 0);
    rcnt.resize(nums[2], 0);
    
    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<left[i].size; j++){
            lcnt[lpos[i]+j]++;
        }
    }
    
    for(unsigned i=0; i<right.size(); i++){
        for(unsigned j=0; j<right[i].size; j++){
            rcnt[rpos[i]+j]++;
        }
    }
}

void Interleave::push(){
    
    for(float &i : middle) i = 0.0f;

    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<left[i].size; j++){
            middle[lpos[i]+j] += left[i][j];
        }
    }

    for(unsigned i=0; i<middle.size(); i++)
        if(lcnt[i]) middle[i] /= lcnt[i];
    
    for(unsigned i=0; i<right.size(); i++){
        for(unsigned j=0; j<right[i].size; j++){
            right[i][j] = middle[rpos[i]+j];
        }
    }
}

void Interleave::pull(){

    if(nopull) return;

    for(float &i : middle) i = 0.0f;

    for(unsigned i=0; i<right.size(); i++){
        for(unsigned j=0; j<right[i].size; j++){
            middle[rpos[i]+j] += right[i][j];
        }
    }
    
    for(unsigned i=0; i<middle.size(); i++)
        if(rcnt[i]) middle[i] /= rcnt[i];
    
    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<left[i].size; j++){
            left[i][j] = middle[lpos[i]+j];
        }
    }
}

namespace Factory { extern std::string interleave; }
std::string Interleave::get_type(){ return Factory::interleave; }

bool Interleave::ok(arrays in, arrays out, args a){
    
    std::vector<unsigned> nums;
    for(auto i : a){
        try {
            nums.push_back(std::stoul(i));
        } catch (const std::invalid_argument &e){}
    }

    if(nums.size() != 5) return 0;
    
    if(nums[0] == 0 || in.size() != nums[0]) return 0;
    for(unsigned i=0; i<nums[0]; i++) if(in[i].size != nums[1]) return 0;
    if(nums[2] < nums[1]) return 0;
    
    if(nums[3] == 0 || out.size() != nums[3]) return 0;
    for(unsigned i=0; i<nums[3]; i++) if(out[i].size != nums[4]) return 0;
    if(nums[2] < nums[4]) return 0;

    return 1;
}

// copy ///////////////////////////////////////////////////////////////////////

void Copy::push(){
    for(unsigned i=0; i<right.size(); i++){
        for(unsigned j=0; j<left[0].size; j++){
            right[i][j] = left[0][j];
        }
    }
}

void Copy::pull(){

    if(nopull) return;

    for(unsigned i=0; i<left[0].size; i++) left[0][i] = 0.0f;

    for(unsigned i=0; i<right.size(); i++){
        for(unsigned j=0; j<left[0].size; j++){
            left[0][j] += right[i][j];
        }
    }
}

namespace Factory { extern std::string copy; }
std::string Copy::get_type(){ return Factory::copy; }

bool Copy::ok(arrays in, arrays out, args a){
    
    if(in.size() != 1 || out.size() < 1) return 0;
    for(auto i : out) if(i.size != in[0].size) return 0;
    return 1;
}

// join ///////////////////////////////////////////////////////////////////////

void Join::push(){

    for(unsigned i=0; i<right[0].size; i++) right[0][i] = 0.0f;

    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<right[0].size; j++){
            right[0][j] += left[i][j];
        }
    }
}

void Join::pull(){

    if(nopull) return;

    for(unsigned i=0; i<left.size(); i++){
        for(unsigned j=0; j<right[0].size; j++){
            left[i][j] = right[0][j];
        }
    }
}

namespace Factory { extern std::string join; }
std::string Join::get_type(){ return Factory::join; }

bool Join::ok(arrays in, arrays out, args a){
    
    if(out.size() != 1 || in.size() < 1) return 0;
    for(auto i : out) if(i.size != out[0].size) return 0;
    return 1;
}

}

