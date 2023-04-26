#ifndef MEMORY_ACCESS
#define MEMORY_ACCESS
#include<stdint.h>
#include "Common.h"
#include "Pipeline.h"
#include "Predictor.h"
#include "Cache.h"
Pipeline mem_access(Pipeline &EX_MA, Cache &$M){
    
    Pipeline MA_WB(EX_MA);
    if(EX_MA.isBubble)
        return MA_WB;
    if(!EX_MA.controls.memRead && !EX_MA.controls.memWrite)
        return MA_WB;
    Instruction inst=EX_MA.inst;
    switch(inst.opcode){
        case 0b0000011:
        switch (inst.funct3) {
            case (0b000): // LB
                MA_WB.ld_res = ((int8_t)$M.lookup(MA_WB.alu_res));
                break;
            case (0b001): // LH
                MA_WB.ld_res = ((int16_t)$M.lookup(MA_WB.alu_res));
                break;
            case (0b010): // LW
                MA_WB.ld_res = ((int32_t)$M.lookup(MA_WB.alu_res));
                break;
            case (0b100): // LBU
                MA_WB.ld_res = ((uint8_t)$M.lookup(MA_WB.alu_res));
                break;
            case (0b101): // LHU
                MA_WB.ld_res = ((uint16_t)$M.lookup(MA_WB.alu_res));
                break;
        }
        break;
        case 0b0100011:
            switch (inst.funct3) {
            case (0b000): // SB
                $M.write(MA_WB.alu_res,  MA_WB.op2 & 0b1111111);
                // memory[MA_WB.alu_res] = MA_WB.op2 & 0b1111111;
                break;
            case (0b001): // SH
                $M.write(MA_WB.alu_res, MA_WB.op2 & 0xffff);
                // memory[MA_WB.alu_res] = MA_WB.op2 & 0xffff;
                break;
            case (0b010): // SW
                $M.write(MA_WB.alu_res, MA_WB.op2);
                // memory[MA_WB.alu_res] = MA_WB.op2;
                break;
        }
        break;
    }
    return MA_WB;
}
#endif