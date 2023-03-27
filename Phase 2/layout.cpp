#include<iostream>
#include "pc.h"
using namespace std;


class Control{
public:
    bool isBranch, isJump;
    bool isRet, isSt, isLd;
    Control(){
        isBranch=0, isJump=0, isRet=0;
    }
};

class Pipeline{
    uint32_t instruction, op2, A, B, branchTarget, alu_res, ld_res;
    Control stmts;
    uint32_t PC;
};
uint8_t instruction_memory[1024];
uint32_t getElement(uint32_t address){
    return *(uint32_t*)(instruction_memory+address);
}
struct inputs{
    PC pc;
    Pipeline IF_DE, DE_EX, EX_MA, MA_WB;
    Control controls;
};
Pipeline fetch(PC &pc, Control &control){
    uint32_t inst=getElement(pc.pc);
    // if(initial.isBranch){
    //     pc+=immB;
    //     initial.isBranch=0;
    // }
    // else if(initial.isJump==1){
    //     pc+=immJ;
    //     initial.isJump=0;
    // }
    // else if(initial.isJump==2){
    //     pc=immJl;
    //     initial.isJump=0;
    // }
    // else{
    //     pc+=4;
    // }
}
Pipeline decode(Pipeline &p, Control &control);
Pipeline execute(Pipeline &p, Control &control);
Pipeline mem_access(Pipeline &p, Control &control);
void writeback(Pipeline &p, Control &control);
void idea(){
    // uint32_t pc;
    inputs inp;
    
    Pipeline IF_DE=fetch(inp.pc, inp.controls);
    Pipeline DE_EX=decode(inp.IF_DE, inp.controls);
    Pipeline EX_MA=execute(inp.DE_EX, inp.controls);
    Pipeline MA_WB=mem_access(inp.EX_MA, inp.controls);
    writeback(inp.MA_WB, inp.controls);
    inp.pc.updatePC();
    inp.DE_EX=IF_DE;
    inp.DE_EX=DE_EX;
    inp.EX_MA=EX_MA;
    inp.MA_WB=MA_WB;
}
int main(){

    return 0;
}