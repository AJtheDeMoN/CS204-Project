#ifndef PIPELINE
#define PIPELINE
#include<inttypes.h>
#include "Common.h"
class Control{
public:
    bool isBranch, memRead, memToReg, ALUop, memWrite, ALUSrc, RegWrite;
    Control(){
        isBranch=memRead=memToReg=ALUop=memWrite=ALUSrc=RegWrite=0;
    }
    Control(Control &rhs){
        isBranch=rhs.isBranch;
        memRead=rhs.memRead;
        memToReg=rhs.memToReg;
        ALUop=rhs.ALUop;
        memWrite=rhs.memWrite;
        ALUSrc=rhs.ALUSrc;
        RegWrite=rhs.RegWrite;
    }
    void ControlUnit(uint32_t opcode){
        isBranch=(opcode==0b1100011 || opcode==0b1101111 || opcode==0b1100111);
        memRead=(opcode==0b0000011);
        memToReg=(opcode==0b0000011);
        ALUop=(opcode==0b0110011 || opcode==0b0010011);
        memWrite=(opcode==0b0100011);
        ALUSrc=(opcode==0b0010011 || opcode==3 || opcode==0b0100011|| opcode==0b1100011 || opcode==0b1101111 || opcode==0b1100111);
        RegWrite=(opcode==0b0110011 || opcode==0b0010011 || opcode==3 || opcode==0b1101111 || opcode==0b1100111 || opcode==0b0110111 || opcode==0b0010111);
    }
};

class Pipeline{
public:
    uint32_t instruction, op2, A, B, branchTarget, alu_res, ld_res;
    Control controls;
    uint32_t pc;
    Instruction inst;
    
    bool isBubble;
    Pipeline():op2(0), instruction(0), A(0), B(0), branchTarget(0), alu_res(0), ld_res(0), pc(0), isBubble(false) {
    };
    Pipeline(Pipeline &rhs){
        instruction=rhs.instruction;
        op2=rhs.op2;
        A=rhs.A;
        B=rhs.B;
        branchTarget=rhs.branchTarget;
        alu_res=rhs.alu_res;
        ld_res=rhs.ld_res;
        pc=rhs.pc;
        inst=rhs.inst;
        controls=Control(rhs.controls);
        isBubble=rhs.isBubble;
    }
};

#endif
