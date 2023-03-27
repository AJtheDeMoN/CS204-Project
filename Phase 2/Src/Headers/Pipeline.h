#ifndef PIPELINE
#define PIPELINE
#include<inttypes.h>
class Control{
public:
    bool isBranch, isJump;
    bool isRet, isSt, isLd;
    Control(){
        isBranch=0, isJump=0, isRet=0;
    }
};

class Pipeline{
public:
    uint32_t instruction, op2, A, B, branchTarget, alu_res, ld_res;
    Control stmts;
    uint32_t pc;
};

#endif
