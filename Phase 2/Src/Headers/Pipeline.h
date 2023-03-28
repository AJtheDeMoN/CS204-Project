#ifndef PIPELINE
#define PIPELINE
#include<inttypes.h>
class Control{
public:
    bool isBranch, isJump;
    bool isRet, isSt, isLd;
    Control(){
        isBranch=isJump=isRet=isSt=isLd=0;
    }
    Control(Control &rhs){
        isBranch=rhs.isBranch;
        isJump=rhs.isJump;
        isRet=rhs.isRet;
        isSt=rhs.isSt;
        isLd=rhs.isLd;
    }
};

class Pipeline{
public:
    uint32_t instruction, op2, A, B, branchTarget, alu_res, ld_res;
    Control controls;
    uint32_t pc;

    Pipeline(){
        instruction=op2=A=B=branchTarget=alu_res=ld_res=pc=0;
        controls=*(new Control());
    };
    Pipeline(Pipeline &rhs){
        instruction=rhs.instruction;
        op2=rhs.op2;
        A=rhs.A;
        B=rhs.B;
        branchTarget=rhs.branchTarget;
        alu_res=rhs.alu_res;
        ld_res=rhs.ld_res;
        controls=(*new Control(rhs.controls));
    }
};

#endif
