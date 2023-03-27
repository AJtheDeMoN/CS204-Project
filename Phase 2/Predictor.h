#ifndef BRANCH_PREDICTOR 
#define BRANCH_PREDICTOR
#include<stdint.h>
#include <vector>
#include<unordered_map>
class Predictor{
    std::unordered_map<uint32_t,uint32_t> hash;
public:
    Predictor(){
        hash.clear();
    }
    bool check(uint32_t pc){
        if(hash.find(pc)==hash.end())
            return false;
        return true;
    }

    void insert(uint32_t pc, uint32_t b_target){
        hash.insert(std::make_pair(pc, b_target));
    }

    void update(uint32_t pc, uint32_t b_target){
        hash[pc]=b_target;
    }

    uint32_t predict(uint32_t pc){
        return hash[pc];
    }
    ~Predictor(){
        hash.clear();
    }
};
#endif