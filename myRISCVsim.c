/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of RISCV Processor

Developer Names:  ABHISHEK JAISWAL  (2021CSB1061)
                  AJAYBEER SINGH    (2021CSB1063)
                  AKANKSH CAIMI     (2021CSB1064)
                  DEVANSHU DHAWAN   (2021CSB1082)
                  
Developer Email ids: ABHISHEK JAISWAL  ->  2021csb1061@iitrpr.ac.in 
                     AJAYBEER SINGH    ->  2021csb1063@iitrpr.ac.in 
                     AKANKSH CAIMI     ->  2021csb1064@iitrpr.ac.in 
                     DEVANSHU DHAWAN   ->  2021csb1082@iitrpr.ac.in 
                     
Date: 

*/

#include<stdio.h>
#include<stdint.h>


typedef struct{
    uint32_t opcode:7;
    uint32_t rd:5;
    uint32_t funct3:3;
    uint32_t rs1:5;
    uint32_t rs2:5;
    uint32_t funct7:7;
    uint32_t imm12:12;
} Instruction;

uint8_t instruction_memory[1024];
uint8_t memory[1024];
uint32_t registers[32];
uint32_t pc=0;
int32_t immB=0,immJ=0,immJl=0;
int isBranch=0,isJump=0;

void store_instructions(){
    FILE *inp=fopen("dump.mc", "r");
    unsigned int address, code;
    while(fscanf(inp, "%x %x", &address, &code)!=EOF){
        *(int*)(instruction_memory+address)=code;
    }
}

uint32_t getElement(int address){
    return *(int*)(instruction_memory+address);
}

void print_instruction(Instruction inst){
    printf("opcode: %x\n", inst.opcode);
    printf("rd: %x\n", inst.rd);
    printf("func3: %x\n", inst.funct3);
    printf("rs1: %x\n", inst.rs1);
    printf("rs2: %x\n", inst.rs2);
    printf("func7: %x\n", inst.funct7);
    printf("imm12: %x\n", inst.imm12);
}

uint32_t fetch(uint32_t* pc,uint32_t* inst){
    if(isBranch){
        pc+=immB;
        isBranch=0;
    }
    else if(isJump==1){
        pc=immJ;
        isJump=0;
    }
    else if(isJump==2){
        pc=immJl;
        isJump=0;
    }
    else{
        pc+=4;
    }
    *inst=getElement(*pc);//i want 32 bits here from memory 
    printf("Fetching instruction %x at pc=%x\n", *inst, *pc);
    return 0;
}
Instruction decode(uint32_t inst,uint32_t* alu_op1,uint32_t* alu_op2){
    Instruction decoded_inst;
    decoded_inst.opcode=inst & 0x7f;
    decoded_inst.rd=(inst>>7)& 0x1f;
    decoded_inst.funct3=(inst>>12)& 0x7;
    decoded_inst.rs1=(inst>>15)& 0x1f;
    decoded_inst.rs2=(inst>>20)& 0x1f;
    decoded_inst.funct7=(inst>>25)& 0x7f;
    decoded_inst.imm12=(inst>>20)& 0xfff;
    *alu_op1=registers[decoded_inst.rs1];
    *alu_op2=registers[decoded_inst.rs2];
    printf("Decoded instruction as:\n");
    print_instruction(decoded_inst);
    return decoded_inst;
}
void execute(Instruction* inst,uint32_t* alu_op1,uint32_t* alu_op2,uint32_t* alu_result){
    int32_t imm;
    printf("executing instruction: ");
    switch(inst->opcode){
        case 0b0110011:
            printf("%x = %d",inst->rd ,*alu_op1);
            switch(inst->funct3){
                case 0b000:
                   if(inst->funct7==0b0000000){//addition
                    printf(" + ");
                    *alu_result=*alu_op1+*alu_op2;
                   }
                   else if(inst->funct7==0b0100000){//sub
                    printf(" - ");
                    *alu_result=*alu_op1+*alu_op2;
                   }
                   break;
                case 0b001:
                    printf(" << ");
                    *alu_result=*alu_op1<<(*alu_op2& 0x1f);
                    break;
                case 0b010://slt
                    printf(" < ");
                    *alu_result=*alu_op1<*alu_op2?1:0;
                    break;
                case 0b011: // SLTU
                    printf(" < ");
                    *alu_result= ((uint32_t)*alu_op1<(uint32_t)*alu_op2)?1:0;
                    break;
                case 0b100: // XOR
                    printf(" ^ ");
                    *alu_result = *alu_op1 ^ *alu_op2;
                    break;
                case 0b101: // SRL, SRA
                    printf(" >> ");
                    if (inst->funct7 == 0b0000000) { // SRL
                        *alu_result=*alu_op1>>(*alu_op2 & 0x1f);
                    } else if (inst->funct7 == 0b0100000) { // SRA
                        *alu_result=(int32_t)*alu_op1>>(*alu_op2 & 0x1f);
                    }
                    break;
                case 0b110: // OR
                    printf(" | ");
                    *alu_result= *alu_op1 | *alu_op2;
                    break; 
                case 0b111: // AND
                    printf(" & ");
                    *alu_result = *alu_op1 & *alu_op2;
                    break;
             }
             printf("%d", *alu_op1);
            break;
        case 0b0010011:
            printf("%d = %d",inst->rd ,*alu_op1);
            imm=(int32_t)((inst->funct7<<5)|(inst->rd));
            switch(inst->funct3){
                case 0b000:
                    printf(" + ");
                    *alu_result=*alu_op1+imm;
                    break;
                case 0b010: // SLTI
                    printf(" < ");
                    *alu_result = *alu_op1 < imm ? 1 : 0;
                    break;
                case 0b011: // SLTIU
                    printf(" < ");
                    *alu_result = ((uint32_t)*alu_op1) < ((uint32_t)imm) ? 1 : 0;
                    break;
                case 0b100: // XORI
                    printf(" ^ ");
                    *alu_result = *alu_op1 ^ imm;
                    break;
                case 0b110: // ORI
                    printf(" | ");
                    *alu_result = *alu_op1 | imm;
                    break;
                case 0b111: // ANDI
                    printf(" & ");
                    *alu_result = *alu_op1 & imm;
                    break;
                case 0b101:
                    if (inst->funct7 == 0b0000000) { // SRLI
                        printf(" << ");
                        *alu_result = *alu_op1 >> (imm & 0x1f);
                    } else { // SRAI
                        printf(" >> ");
                        *alu_result = (int32_t)*alu_op1 >> (imm & 0x1f);
                    }
                    break;
            }
            printf("%d", imm);
            break;
        case 0b1100011://branch instructions
            immB=(int32_t)((inst->funct7<<11)|(inst->imm12<<1));
            switch(inst->funct3){
                case 0b000:
                   if(*alu_op1==*alu_op2){//beq
                    isBranch=1;
                   }
                   break;
                case 0b001://bne
                   if(*alu_op1!=*alu_op2){
                    isBranch=1;
                   }
                   break;
                case 0b100://blt
                    if((int32_t)(*alu_op1)<(int32_t)(*alu_op2)){
                        isBranch=1;
                    }
                    break;
                case 0b101://bge
                    if((int32_t)(*alu_op1)>=(int32_t)(*alu_op2)){
                        isBranch=1;
                    }
            }
            break;
        case 0b1101111://jal
            imm=((inst->imm12&0x800)<<11)|((inst->imm12&0xff000)>>9)|((inst->imm12&0x7fe)>>8)|(inst->funct7<<12);
        
            *alu_result=*alu_op1+4;
            registers[inst->rd]=pc+4;
            isJump=1;
            immJ=(*alu_result&0xffffffe)+imm;
            break;
        case 0b1100111://jalr
            imm=(int32_t)inst->imm12;
            *alu_result=*alu_op1+imm;
            registers[inst->rd]=pc+4;
            isJump=2;
            immJl=(*alu_result&0xffffffe);
            break;
        case 0b0000011:
            imm=(int32_t)((inst->funct7<<5)|(inst->rd));
            *alu_result=*alu_op1+imm;
            break;
        case 0b0100011: // S-type instructions - Store
            imm=(int32_t)((inst->funct7 << 5) | inst->rd) | ((inst->imm12 & 0xfe0) >> 5);
            *alu_result= *alu_op1+ imm;
            break;
    }
}

void memoryRead(Instruction* instruction, uint32_t* alu_result, uint32_t* read, uint32_t* write) {
    switch (instruction->opcode) {
        case 0b0000011: // Load
            switch (instruction->funct3) {
            case 0b000: // LB
                *read = ((int8_t)&memory[*alu_result]);
                break;
            case 0b001: // LH
                *read = ((int16_t)&memory[*alu_result]);
                break;
            case 0b010: // LW
                *read = ((int32_t)&memory[*alu_result]);
                break;
            case 0b100: // LBU
                *read = memory[*alu_result];
                break;
            case 0b101: // LHU
                *read = ((uint16_t)&memory[*alu_result]);
                break;
        }
        break;
        case 0b0100011: //Store
            switch (instruction->funct3) {
            case 0b000: // SB
                //to be written
                break;
            case 0b001: // SH
                //to be written
                break;
            case 0b010: // SW
                //to be written
                break;
        }
        break;
    }
}

void writeback(Instruction* instruction, uint32_t* alu_result, uint32_t* read) {
    switch (instruction->opcode) {
    case 0b0110011: // r
        registers[instruction->rd]=*alu_result;
        break;
    case 0b0010011:
        registers[instruction->rd]=*alu_result;
        break;
    case 0b0000011: // l
        registers[instruction->rd] = *read;
        break;
    case 0b0100011: // s
        //to be written
        break;
    default:
        break;
    }
}

void assign_default_values() {
    for(int i = 0;i<32;i++) registers[i]=0; 
    registers[2]=2147483632;  // stack pointer
}

int main(){
    store_insrtuctions();
    assign_default_values();
    uint32_t instruction;
    fetch(&pc,&instruction);
    uint32_t alu_op1,alu_op2;
    Instruction decoded_instruction=decode(instruction,&alu_op1,&alu_op2);
    //execute instruction
    uint32_t alu_result;
    execute(&decoded_instruction,&alu_op1,&alu_op2,&alu_result);
    uint32_t read,write;//memory read
    memoryRead(&decoded_instruction,&alu_result,&read,&write);
    writeback(&decoded_instruction,&alu_result,&read);//writeback
    return 0;
}