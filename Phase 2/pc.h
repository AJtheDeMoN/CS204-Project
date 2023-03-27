#ifndef pc_class
#define pc_class
#include<stdint.h>
class PC
{
public:
    uint32_t pc, branchTarget;
    PC(){
        pc=0, branchTarget=0;
    }
    void updatePC(){
        pc+=branchTarget;
    }
    // void updateBranch(params){
        // parse params
    // }   
};

#endif