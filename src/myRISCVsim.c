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
                     
Date: 13.03.2023
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
    int32_t imm12:12;
} Instruction;

uint8_t instruction_memory[1024];
uint8_t memory[1024];
uint32_t registers[32];
uint32_t pc=-4;
int32_t immB=0,immJ=0,immJl=0;
int isBranch=0,isJump=0;
int stop=0;
void store_instructions(){
    FILE *inp=fopen("../test/new.txt", "r");
    unsigned int address, code;
    while(fscanf(inp, "%x %x", &address, &code)!=EOF){
        *(uint32_t*)(instruction_memory+address)=code;
    }
}

uint32_t getElement(int address){
    return *(uint32_t*)(instruction_memory+address);
}
uint32_t getElementMemory(uint32_t address){
    return *(uint32_t*)(memory+address);
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
        *pc+=immB;
        isBranch=0;
    }
    else if(isJump==1){
        *pc=immJ;
        isJump=0;
    }
    else if(isJump==2){
        *pc=immJl;
        isJump=0;
    }
    else{
        *pc+=4;
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
        case (0b0110011):
            printf("%x = %d",inst->rd ,*alu_op1);
            switch(inst->funct3){
                case (0b000):
                   if(inst->funct7==(0b0000000)){//addition
                    printf(" + ");
                    *alu_result=*alu_op1+*alu_op2;
                   }
                   else if(inst->funct7==(0b0100000)){//sub
                    printf(" - ");
                    *alu_result=*alu_op1+*alu_op2;
                   }
                   break;
                case (0b001):
                    printf(" << ");//sll
                    *alu_result=*alu_op1<<(*alu_op2& 0x1f);
                    break;
                case (0b010)://slt
                    printf(" < ");
                    *alu_result=*alu_op1<*alu_op2?1:0;
                    break;
                case (0b011): // SLTU
                    printf(" < ");
                    *alu_result= ((uint32_t)*alu_op1<(uint32_t)*alu_op2)?1:0;
                    break;
                case (0b100): // XOR
                    printf(" ^ ");
                    *alu_result = *alu_op1 ^ *alu_op2;
                    break;
                case (0b101): // SRL, SRA
                    printf(" >> ");
                    if (inst->funct7 == (0b0000000)) { // SRL
                        *alu_result=*alu_op1>>(*alu_op2 & 0x1f);
                    } else if (inst->funct7 == (0b0100000)) { // SRA
                        *alu_result=(int32_t)*alu_op1>>(*alu_op2 & 0x1f);
                    }
                    break;
                case (0b110): // OR
                    printf(" | ");
                    *alu_result= *alu_op1 | *alu_op2;
                    break; 
                case (0b111): // AND
                    printf(" & ");
                    *alu_result = *alu_op1 & *alu_op2;
                    break;
             }
             printf("%d", *alu_op1);
            break;
        case (0b0010011): // i
            printf("%d = %d",inst->rd ,*alu_op1);
            // imm=(int32_t)((inst->funct7<<5)+(inst->rd));
            imm=(int32_t)inst->imm12;
            switch(inst->funct3){
                case (0b000):
                    printf(" + "); //addi
                    *alu_result=*alu_op1+imm;
                    break;
                case (0b010): // SLTI
                    printf(" < ");
                    *alu_result = *alu_op1 < imm ? 1 : 0;
                    break;
                case (0b011): // SLTIU
                    printf(" < ");
                    *alu_result = ((uint32_t)*alu_op1) < ((uint32_t)imm) ? 1 : 0;
                    break;
                case (0b100): // XORI
                    printf(" ^ ");
                    *alu_result = *alu_op1 ^ imm;
                    break;
                case (0b110): // ORI
                    printf(" | ");
                    *alu_result = *alu_op1 | imm;
                    break;
                case (0b111): // ANDI
                    printf(" & ");
                    *alu_result = *alu_op1 & imm;
                    break;
                case (0b101):
                    if (inst->funct7 == (0b0000000)) { // SRLI
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
        case (0b1100011)://branch instructions
            // immB=(int32_t)((inst->funct7<<11)|(inst->imm12<<1));
            immB=(inst->funct7&(0b111111))<<5;
            immB+=((inst->funct7>>6)&1)<<12;
            immB+=(inst->rd&1)<<11;
            immB+=(inst->rd&(0b11110));
            if((immB>>12)&1)
                immB+=0b11111111111111111110000000000000;
            switch(inst->funct3){
                printf("%d ",inst->rs1);
                case (0b000):
                   if(*alu_op1==*alu_op2){//beq
                    printf("==");
                    isBranch=1;
                   }
                   break;
                case (0b001)://bne
                   if(*alu_op1!=*alu_op2){
                    printf("!=");
                    isBranch=1;
                   }
                   break;
                case (0b100)://blt
                    if((int32_t)(*alu_op1)<(int32_t)(*alu_op2)){
                        printf("<");
                        isBranch=1;
                    }
                    break;
                case (0b101)://bge
                    if((int32_t)(*alu_op1)>=(int32_t)(*alu_op2)){
                        printf(">=");
                        isBranch=1;
                    }
                    break;
                printf(" %d",inst->rs2);
            }
            break;
        case (0b1101111)://jal
            imm=((inst->imm12&0x800)<<11)|((inst->imm12&0xff000)>>9)|((inst->imm12&0x7fe)>>8)|(inst->funct7<<12);
            printf("%d = %d + 4",inst->rd,pc);
            *alu_result=*alu_op1+4;
            registers[inst->rd]=pc+4;
            isJump=1;
            immJ=(*alu_result&0xffffffe)+imm;
            break;
        case (0b1100111)://jalr
            printf("%d = %d + 4",inst->rd,pc);
            imm=(int32_t)inst->imm12;
            *alu_result=*alu_op1+imm;
            registers[inst->rd]=pc+4;
            isJump=2;
            immJl=(*alu_result&0xffffffe);
            break;
        case (0b0000011)://l
            // imm=(int32_t)((inst->funct7<<5)|(inst->rd));
            imm=(int32_t)inst->imm12;
            printf("%d + %d", inst->rs1, imm);
            *alu_result=*alu_op1+imm;
            break;
        case (0b0100011): // S-type instructions - Store
            imm=(int32_t)((inst->funct7 << 5) | inst->rd) | ((inst->imm12 & 0xfe0) >> 5);
            printf("%d + %d", inst->rs1, imm);
            *alu_result= *alu_op1+ imm;
            break;
    }
    printf("\n");
}

void memoryRead(Instruction* instruction, uint32_t* alu_result, uint32_t* read, uint32_t* write) {
    printf("Reading from memory\n");
    switch (instruction->opcode) {
        case (0b0000011): // Load
            switch (instruction->funct3) {
            case (0b000): // LB
                *read = ((int8_t)getElementMemory(*alu_result));
                break;
            case (0b001): // LH
                *read = ((int16_t)getElementMemory(*alu_result));
                break;
            case (0b010): // LW
                *read = ((int32_t)getElementMemory(*alu_result));
                break;
            case (0b100): // LBU
                *read = ((uint8_t)getElementMemory(*alu_result));
                break;
            case (0b101): // LHU
                *read = ((uint16_t)getElementMemory(*alu_result));
                break;
        }
        break;
        case (0b0100011): {//Store
            switch (instruction->funct3) {
            case (0b000): // SB
                memory[*alu_result] = registers[instruction->rs2] & 0b1111111;
                break;
            case (0b001): // SH
                memory[*alu_result] = registers[instruction->rs2] & 0xffff;
                break;
            case (0b010): // SW
                 memory[*alu_result] = registers[instruction->rs2];
                break;
        }
        break;}
    }
}

void writeback(Instruction* instruction, uint32_t* alu_result, uint32_t* read) {
    printf("Writing to memory\n");
    switch (instruction->opcode) {
    case (0b0110011): // r
        registers[instruction->rd]=*alu_result;
        break;
    case (0b0010011): //i
        registers[instruction->rd]=*alu_result;
        break;
    case (0b0000011): // i
        registers[instruction->rd] = *read;
        break;
    default:
        break;
    }
}

void assign_default_values() {
    for(int i = 0;i<32;i++) registers[i]=0; 
    registers[2]=0x90;  // stack pointer
}

void show_memory(){
    FILE *temp = fopen("memory.txt","w");
    for(int i=0; i<1024; i+=4){
        uint32_t x=*(uint32_t*)(i + memory);
        char str[20];
        sprintf(str, "0x%x 0x%x\n", i, x);
        fputs(str, temp);
    }
}

void show_register(){
    FILE *temp = fopen("register.txt","w");
    for(int i=0; i<32; i++){
        char str[20];
        sprintf(str, "0x%x 0x%x\n", i, registers[i]);
        fputs(str, temp);
    }
}

int main(){
    store_instructions();
    assign_default_values();
    while(1){
        uint32_t instruction;
        fetch(&pc,&instruction);
        uint32_t alu_op1,alu_op2;
        Instruction decoded_instruction=decode(instruction,&alu_op1,&alu_op2);
        if(decoded_instruction.opcode==(0x7f)){
            break;
        }
        //execute instruction
        uint32_t alu_result;
        execute(&decoded_instruction,&alu_op1,&alu_op2,&alu_result);
        uint32_t read,write;//memory read
        memoryRead(&decoded_instruction,&alu_result,&read,&write);
        writeback(&decoded_instruction,&alu_result,&read);//writeback
    }
    show_memory();
    show_register();
    return 0;
}
