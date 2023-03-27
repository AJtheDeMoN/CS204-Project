#ifndef FETCH 
#define FETCH
#include<stdint.h>
#include "Pipeline.h"
#include "Predictor.h"
#include "Common.h"
Pipeline fetch(uint32_t &pc, Predictor &p){

    if(p.check(pc))
        pc=p.predict(pc);
    Pipeline obj;
    obj.pc=pc, obj.instruction=getInstruction(pc);
    return obj;
}   
#endif