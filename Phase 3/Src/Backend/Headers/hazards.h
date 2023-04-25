#ifndef HAZARDS
#define HAZARDS
#include "Pipeline.h"
#include "Common.h"
#include <iostream>
using namespace std;
int data_hazard=0;
// This function checks for hazard between two stages in a pipeline based on their insttructions opcode and register usage
bool checkHazardRS1(Pipeline &in, Pipeline &out){
    //Extract opcode from the two instructions
    uint32_t opcode1=in.instruction&(0x7f);
    uint32_t opcode2=out.instruction&(0x7f);
    //check whether any of the instruction is a bubble or NOP
    if (opcode1==0 || opcode2==0 || in.isBubble || out.isBubble)
        return false;
    // Check if instruction1 is a JAL,LUI or AUIPC
    if (opcode1 == 0b1101111 || opcode1 ==0b0110111 || opcode1==0b0010111)
        return false;
    // Check if instruction2 is a store or branch instruction
    if (opcode2 == 0b0100011 || opcode2 == 0b1100011)
        return false;
    
    // Extracting the rs1 value in inst1 and destination register of inst2
    uint32_t rs1=(in.instruction>>15)&(0x1f);
    uint32_t rd=(out.instruction>>7)&(0x1f);

    // Check if there is a hazard by comparing source and destination registers
    if(rs1==rd){
        data_hazard++;
        return true;}
    return false;
}

// This function checks for a hazard between two stages in a pipeline based on their instructions' opcode and register usage
bool checkHazardRS2(Pipeline &in, Pipeline &out){
    // Extract opcode from instruction
    uint32_t opcode1=in.instruction&(0x7f);
    uint32_t opcode2=out.instruction&(0x7f);
    // Check if any of the instructions is a bubble or NOP
    if (opcode1==0 || opcode2==0)
        return false;
    // Check if instr1 is a JAL,LUI,AUIPC,ADDI,LOAD or JALR
    if (opcode1 == 0b1101111 || opcode1 ==0b0110111 || opcode1==0b0010111 || opcode1 == 0b0010011 || opcode1 == 0b0000011 || opcode1 == 0b1100111)
        return false;
    // Check if instruction2 is a store or branch instruction
    if (opcode2 == 0b0100011 || opcode2 == 0b1100011)
        return false;
    
    // Extract source and destination registers from instructions
    uint32_t rs2=(in.instruction>>20)&(0x1f);
    uint32_t rd=(out.instruction>>7)&(0x1f);
    // Check if there is a hazard by comparing source and destination registers
    if(rs2==rd){
        data_hazard++;
        return true;}
    return false;
}


//This function forwards data between pipeline stages to avoid hazards.
void forward (Pipeline &p1, Pipeline &p2, Pipeline &p3, Pipeline &p4){
    // if(checkHazardRS1(p3,p4)){
    // }
    // Hazard detection for RS1 and stage p1-p2
    if(checkHazardRS1(p1,p2) && p2.inst.opcode==3 && !p2.isBubble){
        uint16_t opcode=p1.instruction&0x7f;
        bool ALUop=(opcode==0b0110011 || opcode==0b0010011 || opcode==0b1100011);
        if(ALUop && !p1.isStalled)
            p1.isStall=1, p1.isStalled=true;
    }
    // Forwarding for RS2
    if(checkHazardRS2(p3,p4) && !p4.isBubble){
        p3.op2=p4.ld_res;
    }
    
    // Hazard detection for RS1 and stage p2-p4
    if(checkHazardRS1(p2,p4) && p4.controls.ALUop && !p4.isBubble){
        p2.A=p4.alu_res;
    }
    else if(checkHazardRS1(p2,p4) && !p4.isBubble){
        p2.A=p4.ld_res;
    }
    // Hazard detection for RS2 and stage p2-p4
    if(checkHazardRS2(p2,p4) && p4.controls.ALUop && !p3.isBubble){
        p2.op2=p4.alu_res;
    }
    else if(checkHazardRS2(p2,p4) && !p4.isBubble){
        p2.op2=p4.ld_res;
    }
    // Hazard detection for RS1 and stage p3-p4
    if(checkHazardRS1(p2, p3) && p3.controls.ALUop && !p3.isBubble){
        p2.A=p3.alu_res;
    }
    // Hazard detection for RS2 and stage p3-p4
    if(checkHazardRS2(p2,p3) && p3.controls.ALUop && !p3.isBubble){
        p2.op2=p3.alu_res;
    }
}
void print(Pipeline &p1){
    cout<<"Instruction-> 0x"<<hex<<p1.instruction<<"\n";
    cout<<"Op2->"<<p1.op2<<"\n";
    cout<<"A->"<<p1.A<<"\n";
    cout<<"B->"<<p1.B<<"\n";
    cout<<"Branch Target->"<<p1.branchTarget<<"\n";
    cout<<"Alu_Result->"<<p1.alu_res<<"\n";
    cout<<"Ld_Result->"<<p1.ld_res<<"\n";
    cout<<"PC->"<<p1.pc<<"\n";
    cout<<"ISBubble?->"<<p1.isBubble<<"\n";
    cout<<"IsStall?->"<<p1.isStall<<"\n";
    cout<<"IsStalled?->"<<p1.isStalled<<"\n";
}

void print_registers(int clock){
    cout<<"\nValues stored in registers after "<<clock<<" cycles->\n";
    for(int i=0;i<32;i++){
        cout<<"register["<<i<<"]-> "<<registers[i]<<"\n";
    }
}

void updateMem(){//updating the values in memory.txt
    FILE *mem=fopen("memory.txt", "w");
    for(int i=0; i<1024; i+=4){
        uint32_t temp=*(uint32_t*)(i+memory);
        char str[20];
        sprintf(str, "0x%x 0x%x\n", i, temp);
        fputs(str, mem);
    }
    fclose(mem);
}
void updateReg(){//updating the values in memory.txt
    FILE *mem=fopen("register.txt", "w");
    for(int i=0; i<32; i++){
        // uint32_t temp=*(uint32_t*)(i+memory);
        char str[20];
        sprintf(str, "0x%x 0x%x\n", i, registers[i]);
        fputs(str, mem);
    }
    fclose(mem);
}
void updateClock(uint32_t clock){
    FILE *mem=fopen("clock.txt", "w");
    char str[20];
    sprintf(str, "0x%x\n", clock);
    fputs(str, mem);
    fclose(mem);
}
void assign_registers(){
    registers[2]=0x90;
}

#endif