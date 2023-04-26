#ifndef FETCH 
#define FETCH
#include<stdint.h>
#include "Pipeline.h"
#include "Predictor.h"
#include "Common.h"
#include "Cache.h"

Pipeline fetch(uint32_t &pc, Predictor &p, Cache &$I){
    
    // if(p.check(pc))
    //     pc=p.predict(pc);
    Pipeline obj;
    obj.pc=pc, obj.instruction=$I.lookup(pc);
    return obj;
}   
#endif