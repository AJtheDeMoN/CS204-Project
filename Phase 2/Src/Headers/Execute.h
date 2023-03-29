#ifndef EXECUTE
#define EXECUTE
#include<stdint.h>
#include "Common.h"
#include "Pipeline.h"
#include "Predictor.h"

Pipeline execute(Pipeline &DE_EX, Predictor &p){
    Pipeline EX_MA(DE_EX);
    Instruction inst = DE_EX.inst;
    switch(inst.opcode){
        case (0b0110011):
        case (0b0010011):
            switch(inst.funct3){
                case (0b000):
                   if(inst.funct7==(0b0000000)){
                    // add addi
                    EX_MA.alu_res=DE_EX.A+DE_EX.B;
                   }
                   else if(inst.funct7==(0b0100000)){
                    //sub
                    EX_MA.alu_res=DE_EX.A-DE_EX.B;
                   }
                   break;
                case (0b001)://sll slli
                    EX_MA.alu_res=DE_EX.A<<(DE_EX.B& 0x1f);
                    break;
                case (0b010)://slt
                    EX_MA.alu_res=DE_EX.A<DE_EX.B?1:0;
                    break;
                case (0b011): // SLTU
                    EX_MA.alu_res= ((uint32_t)DE_EX.A<(uint32_t)DE_EX.B)?1:0;
                    break;
                case (0b100): // XOR XORI
                    EX_MA.alu_res = DE_EX.A ^ DE_EX.B;
                    break;
                case (0b101): 
                    // SRL SRLI
                    if (inst.funct7 == (0b0000000)) {
                        EX_MA.alu_res=DE_EX.A>>(DE_EX.B & 0x1f);
                    }
                    // SRA SRAI
                    else if (inst.funct7 == (0b0100000)) {
                        EX_MA.alu_res=(int32_t)DE_EX.A>>(DE_EX.B & 0x1f);
                    }
                    break;
                case (0b110): // OR ORI
                    EX_MA.alu_res= DE_EX.A | DE_EX.B;
                    break; 
                case (0b111): // AND ANDI
                    EX_MA.alu_res = DE_EX.A & DE_EX.B;
                    break;
             }
            break;
        case (0b1100011)://branch instructions
            switch(inst.funct3){
                case (0b000):
                   if(DE_EX.A==DE_EX.op2){//beq
                    EX_MA.controls.isBranch=1;
                    EX_MA.branchTarget=(int32_t)DE_EX.B+DE_EX.pc;
                   }
                   else {
                    DE_EX.controls.isBranch=0;
                    EX_MA.branchTarget=DE_EX.pc+4;
                   }
                   break;
                case (0b001)://bne
                   if(DE_EX.A!=DE_EX.op2){
                    EX_MA.controls.isBranch=1;
                    EX_MA.branchTarget=DE_EX.B+DE_EX.pc;
                    // DE_EX.controls.
                   }
                   else {
                    DE_EX.controls.isBranch=0;
                    EX_MA.branchTarget=DE_EX.pc+4;
                   }
                   break;
                case (0b100)://blt
                    if((int32_t)(DE_EX.A)<(int32_t)(DE_EX.op2)){
                        DE_EX.controls.isBranch=1;
                    }
                    else {
                    DE_EX.controls.isBranch=0;
                    EX_MA.branchTarget=DE_EX.pc+4;
                   }
                    break;
                case (0b101)://bge
                    if((int32_t)(DE_EX.A)>=(int32_t)(DE_EX.op2)){
                        DE_EX.controls.isBranch=1;
                    }
                    else {
                    DE_EX.controls.isBranch=0;
                    EX_MA.branchTarget=DE_EX.pc+4;
                   }
                    break;   
            }
            if(p.check(DE_EX.pc))
                p.update(DE_EX.pc, EX_MA.branchTarget);
            else
                 p.insert(DE_EX.pc, EX_MA.branchTarget);
            break;
        case (0b1101111)://jal
            EX_MA.controls.isBranch=1;
            EX_MA.branchTarget=DE_EX.pc+DE_EX.B;
            EX_MA.alu_res=DE_EX.A+4;
            if(p.check(DE_EX.pc))
                p.update(DE_EX.pc, EX_MA.branchTarget);
            else
                 p.insert(DE_EX.pc, EX_MA.branchTarget);
            break;
        case (0b1100111)://jalr
            EX_MA.controls.isBranch=1;
            EX_MA.branchTarget=DE_EX.A+DE_EX.B;
            EX_MA.alu_res=DE_EX.A+4;
            if(p.check(DE_EX.pc))
                p.update(DE_EX.pc, EX_MA.branchTarget);
            else
                 p.insert(DE_EX.pc, EX_MA.branchTarget);
            break;
        case (0b0000011):// I-type
            EX_MA.alu_res=DE_EX.A+DE_EX.B;
            break;
        case (0b0100011): // S-type            
            EX_MA.alu_res= DE_EX.A+ DE_EX.B;
            break;
    }
    
    return EX_MA;

}
#endif