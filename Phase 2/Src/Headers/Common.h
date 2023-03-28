#ifndef COMMONERS 
#define COMMONERS
#include<inttypes.h>
#include<stdio.h>

uint8_t instruction_memory[1024];
uint8_t memory[1024];
uint32_t registers[32];
void store_instructions(char* location){
    FILE *inp=fopen(location, "r");
    unsigned int address, code;
    while(fscanf(inp, "%x %x",&address,&code)!=EOF){
        *(uint32_t*)(instruction_memory+address)=code;
    }
}
uint32_t getInstruction(int address){
    return *(uint32_t*)(instruction_memory+address);
}
uint32_t getMemory(uint32_t address){
    return *(uint32_t*)(memory+address);
}

struct Instruction{
    uint32_t opcode:7;
    uint32_t rd:5;
    uint32_t funct3:3;
    uint32_t rs1:5;
    uint32_t rs2:5;
    uint32_t funct7:7;
    int32_t imm12:12;

    Instruction(){
        opcode=rd=funct3=rs1=rs2=funct7=imm12=0;
    }
    Instruction(Instruction &rhs){
        opcode=rhs.opcode;
        rd=rhs.rd;
        funct3=rhs.funct3;
        rs1=rhs.rs1;
        rs2=rhs.rs2;
        funct7=rhs.funct7;
        imm12=rhs.imm12;
    }
};

int32_t imm_extender(uint32_t instruction, uint32_t opcode=0){
    if(opcode==0)
        opcode=instruction&(0b1111111);
    int32_t imm2=0;
    switch (opcode)
    {
    //i
    case 0b0010011:
    case 0b1100111:
    case 0b0000011:
        imm2=(instruction>>20)<<20;
        imm2>>=20;
        break;
    //s
    case 0b0100011:
        imm2=(instruction>>25)<<5;
        imm2+=(instruction>>7) & 0b11111;
        imm2=(imm2<<20)>>20;
        break;
    //b
    case 0b1100011:
        imm2=((instruction>>25)&(0b111111))<<5;
        imm2+=((instruction>>31)&1)<<12;
        imm2+=((instruction>>7)&1)<<11;
        imm2+=((instruction>>7)&(0b11110));
        imm2=(imm2<<20)>>20;
        break;
    //u
    case 0b0110111:
    case 0b0010111:
        imm2=instruction>>12;
        imm2=(imm2<<12)>>12;
        break;
    //j
    case 0b1101111:
    imm2=(((instruction>>31)&0x1)<<20)|(((instruction>>12)&0xff)<<12)|(((instruction>>20)&0x1)<<11)|(((instruction>>21)&0x3ff)<<1);
    imm2=(imm2<<11)>>11;
        break;
    }
    return imm2;
}
#endif