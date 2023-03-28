#ifndef DECODE 
#define DECODE
#include<stdint.h>
#include "Pipeline.h"
#include "Predictor.h"
#include "Common.h"
Control ControlUnit(uint32_t opcode){
    Control ctrl;
    

}

Pipeline decode(Pipeline &IF_DE){
    uint32_t inst=IF_DE.instruction;
    Instruction decoded_inst;
    decoded_inst.opcode=inst & 0x7f;
    decoded_inst.rd=(inst>>7)& 0x1f;
    decoded_inst.funct3=(inst>>12)& 0x7;
    decoded_inst.rs1=(inst>>15)& 0x1f;
    decoded_inst.rs2=(inst>>20)& 0x1f;
    decoded_inst.funct7=(inst>>25)& 0x7f;
    decoded_inst.imm12=(inst>>20)& 0xfff;

    Pipeline DE_EX(IF_DE);
    DE_EX.controls=ControlUnit(decoded_inst.opcode);
    DE_EX.A=registers[decoded_inst.rs1];
    DE_EX.op2=registers[decoded_inst.rs2];
}   
#endif