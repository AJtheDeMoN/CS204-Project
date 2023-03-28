#include <iostream>
#include "./Headers/Common.h"
#include "./Headers/Execute.h"
#include "./Headers/Decode.h"
#include "./Headers/MemAccess.h"
#include "./Headers/Writeback.h"
#include "./Headers/Pipeline.h"
using namespace std;

Pipeline foo(Pipeline a){
    Pipeline obj(a);
    a.alu_res=69;
    return obj;
}

int main(){
    Predictor p;
    Pipeline obj1, obj2, obj3, obj4;
    obj1.pc=0, obj1.instruction=0xF89FF96F;
    obj2=decode(obj1);
    obj3=execute(obj2, p);
    obj4=mem_access(obj3);
    writeback(obj4);
    return 0;
}