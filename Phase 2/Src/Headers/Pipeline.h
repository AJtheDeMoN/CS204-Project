#ifndef PIPELINE
#define PIPELINE
#include<inttypes.h>
class Control{
public:
    bool isBranch, isJump;
//  bool isAdd, isSub, isCmp, isMul, isDiv, isMod, isLsl, isLsr, isAsr, isOr, isAnd, isNot, isMov;
    bool isSt, isLd, isBeq, isBgt, isRet, isImmediate, isWb, isUBranch, isCall;
    Control(){
        isBranch=isJump=0;
        isSt=isLd=isBeq=isBgt=isRet=isImmediate=isWb=isUBranch=isCall=0;
//      isAdd=isSub=isCmp=isMul=isDiv=isMod=isLsl=isLsr=isAsr=isOr=isAnd=isNot=isMov=0;
    }
    Control(Control &rhs){
        isBranch=rhs.isBranch;
        isJump=rhs.isJump;
        isSt=rhs.isSt;
        isLd=rhs.isLd;
        isBeq=rhs.isBeq;
        isBgt=rhs.isBgt;
        isRet=rhs.isRet;
        isImmediate=rhs.isImmediate;
        isWb=rhs.isWb;
        isUBranch=rhs.isUBranch;
        isCall=rhs.isCall;
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
