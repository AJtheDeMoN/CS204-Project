#ifndef WRITE_BACK
#define WRITE_BACK
#include<stdint.h>
#include "Common.h"
#include "Pipeline.h"
#include "Predictor.h"

void writeback(Pipeline &MA_WB){
    if(!MA_WB.controls.memToReg && !MA_WB.controls.memWrite)
        return;
    Instruction inst=MA_WB.inst;
    switch (inst.opcode) {
        case (0b0110011): // r
        case (0b0010011): //i
        registers[inst.rd]=MA_WB.alu_res;
        break;
        case (0b0000011): // i
        registers[inst.rd]=MA_WB.ld_res;
        break;
        case (0b1101111):
        case (0b1100111):
        registers[inst.rd]=MA_WB.pc+4;
        break;
    }
    registers[0]=0;
}
#endif