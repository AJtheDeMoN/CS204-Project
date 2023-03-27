#ifndef PC_CLASS
#define PC_CLASS
#include<stdint.h>
#include "Predictor.h"
class PC
{
public:
    uint32_t pc;
    PC(){
        pc=0;
    }
    void updatePC(Predictor &p){
        if(p.check(pc))
            pc=p.predict(pc);
        else pc+=4;
    }
    // void updateBranch(params){
        // parse params
    // }   
};

#endif