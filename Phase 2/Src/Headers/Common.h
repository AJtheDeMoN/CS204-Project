#ifndef COMMONERS 
#define COMMONERS
#include<inttypes.h>
#include<stdio.h>

uint8_t instruction_memory[1024];
uint8_t memory[1024];

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
#endif