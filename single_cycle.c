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

uint8_t memory[1024];
uint32_t registers[32];
uint32_t pc=0;
int32_t immB=0,immJ=0,immJl=0;
int isBranch=0,isJump=0;

uint32_t fetch(uint32_t* pc,uint32_t* inst){
    *inst=0;//i want 32 bits here from memory 
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
    return decoded_inst;
}
void execute(Instruction* inst,uint32_t* alu_op1,uint32_t* alu_op2,uint32_t* alu_result){
    int32_t imm;
    switch(inst->opcode){
        case 0b0110011:
            switch(inst->funct3){
                case 0b000:
                   if(inst->funct7==0b0000000){//addition
                    *alu_result=*alu_op1+*alu_op2;
                   }
                   else if(inst->funct7==0b0100000){//sub
                    *alu_result=*alu_op1+*alu_op2;
                   }
                   break;
                case 0b001:
                    *alu_result=*alu_op1<<(*alu_op2& 0x1f);
                    break;
                case 0b010://slt
                    *alu_result=*alu_op1<*alu_op2?1:0;
                    break;
                case 0b011: // SLTU
                    *alu_result= ((uint32_t)*alu_op1<(uint32_t)*alu_op2)?1:0;
                    break;
                case 0b100: // XOR
                    *alu_result = *alu_op1 ^ *alu_op2;
                    break;
                case 0b101: // SRL, SRA
                    if (inst->funct7 == 0b0000000) { // SRL
                        *alu_result=*alu_op1>>(*alu_op2 & 0x1f);
                    } else if (inst->funct7 == 0b0100000) { // SRA
                        *alu_result=(int32_t)*alu_op1>>(*alu_op2 & 0x1f);
                    }
                    break;
                case 0b110: // OR
                    *alu_result= *alu_op1 | *alu_op2;
                    break; 
                case 0b111: // AND
                    *alu_result = *alu_op1 & *alu_op2;
                    break;
             }
            break;
        case 0b0010011:
            imm=(int32_t)((inst->funct7<<5)|(inst->rd));
            switch(inst->funct3){
                case 0b000:
                    *alu_result=*alu_op1+imm;
                    break;
                case 0b010: // SLTI
                    *alu_result = *alu_op1 < imm ? 1 : 0;
                    break;
                case 0b011: // SLTIU
                    *alu_result = ((uint32_t)*alu_op1) < ((uint32_t)imm) ? 1 : 0;
                    break;
                case 0b100: // XORI
                    *alu_result = *alu_op1 ^ imm;
                    break;
                case 0b110: // ORI
                    *alu_result = *alu_op1 | imm;
                    break;
                case 0b111: // ANDI
                    *alu_result = *alu_op1 & imm;
                    break;
                case 0b101:
                    if (inst->funct7 == 0b0000000) { // SRLI
                        *alu_result = *alu_op1 >> (imm & 0x1f);
                    } else { // SRAI
                        *alu_result = (int32_t)*alu_op1 >> (imm & 0x1f);
                    }
                    break;
            }
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
    }
}



int main(){
    for(int i=0;i<1024;i++){
        memory[i]=(uint8_t)0;
    }
    for(int i=0;i<32;i++){
        registers[i]=0;
    }
    uint32_t instruction;
    fetch(&pc,&instruction);
    uint32_t alu_op1,alu_op2;
    Instruction decoded_instruction=decode(instruction,&alu_op1,&alu_op2);
    //execute instruction
    uint32_t alu_result;
    execute(&decoded_instruction,&alu_op1,&alu_op2,&alu_result);
    
    return 0;
}
