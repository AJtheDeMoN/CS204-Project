#include <iostream>
#include <vector>
#include <sstream>
#include "./Headers/Predictor.h"
#include "./Headers/Pipeline.h"
#include "./Headers/Common.h"
#include "./Headers/Fetch.h"
#include "./Headers/Decode.h"
#include "./Headers/Execute.h"
#include "./Headers/MemAccess.h"
#include "./Headers/Writeback.h"
using namespace std;

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
    if(rs1==rd)
        return true;
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
    if(rs2==rd)
        return true;
    return false;
}



void forward (Pipeline &p1, Pipeline &p2, Pipeline &p3, Pipeline &p4){
    // if(checkHazardRS1(p3,p4)){
    // }
    if(checkHazardRS1(p1,p2) && p2.inst.opcode==3 && !p2.isBubble){
        uint16_t opcode=p1.instruction&0x7f;
        bool ALUop=(opcode==0b0110011 || opcode==0b0010011 || opcode==0b1100011);
        if(ALUop && !p1.isStalled)
            p1.isStall=1, p1.isStalled=true;
    }
    if(checkHazardRS2(p3,p4) && !p4.isBubble){
        p3.op2=p4.ld_res;
    }
    if(checkHazardRS1(p2,p4) && p4.controls.ALUop && !p4.isBubble){
        p2.A=p4.alu_res;
    }
    else if(checkHazardRS1(p2,p4) && !p4.isBubble){
        p2.A=p4.ld_res;
    }
    if(checkHazardRS2(p2,p4) && p4.controls.ALUop && !p3.isBubble){
        p2.op2=p4.alu_res;
    }
    else if(checkHazardRS2(p2,p4) && !p4.isBubble){
        p2.op2=p4.ld_res;
    }
    if(checkHazardRS1(p2, p3) && p3.controls.ALUop && !p3.isBubble){
        p2.A=p3.alu_res;
    }
    if(checkHazardRS2(p2,p3) && p3.controls.ALUop && !p3.isBubble){
        p2.op2=p3.alu_res;
    }
}
void updateMem(){
    FILE *mem=fopen("memory.txt", "w");
    for(int i=0; i<1024; i+=4){
        uint32_t temp=*(uint32_t*)(i+memory);
        char str[20];
        sprintf(str, "0x%x 0x%x\n", i, temp);
        fputs(str, mem);
    }
    fclose(mem);
}
int main(int argv, char** argc){
    vector<string> args;
    for(int i=2; i<argv; i++){
        args.push_back(argc[i]);
    }
    int knobs[5]{};
    //    0                 1             2          3                4
    // pipeline   stalls/forwarding  print_reg   print_pipe_all  print_pipe_x
    for(int i=0; i<args.size(); i++){
        string s=args[i];
        if(s=="-p")
            knobs[0]=1;
        else if(s=="-s")
            knobs[1]=1;
        else if(s=="-v"){
            i++;
            while(i<args.size()){
                if(args[i]=="r")
                    knobs[2]=1;
                else if(args[i]=="p")
                    knobs[3]=1;
                else{
                    char *p;
                    int x=strtol(args[i].c_str(), &p, 10);
                    if(*p)
                        break;
                    else
                        knobs[4]=x;
                }
                i++;
            }
            i--;
        }
    }  
    // store_instructions(argc[1]);
    store_instructions("inst.txt");
    uint32_t pc=0;
    uint32_t clock=1;
    Pipeline IF_DE, DE_EX, EX_MA, MA_WB;
    Predictor p;
    while(1){
        if(!knobs[0]){
            if(MA_WB.isStall>0){
                MA_WB.isStall--;
            }
            else
                MA_WB.isStalled=false;
                writeback(MA_WB);
            if(EX_MA.isStall>0)
                EX_MA.isStall--;
            else
                EX_MA.isStalled=false;
                MA_WB=mem_access(EX_MA);
            if(DE_EX.isStall>0)
                DE_EX.isStall--;
            else
                DE_EX.isStalled=false;  
                EX_MA=execute(DE_EX, p);
            if(IF_DE.isStall>0)
                IF_DE.isStall--;
            else{
                IF_DE.isStalled=false;
                DE_EX=decode(IF_DE);
                IF_DE=fetch(pc, p);
                // update pc
                pc=p.predict(pc);
                if(EX_MA.controls.isBranch && !EX_MA.isBubble && EX_MA.branchTarget!=DE_EX.pc){
                    pc=EX_MA.branchTarget;
                    IF_DE.isBubble=DE_EX.isBubble=true;
                }
            }

            clock++;
            if(clock==95)
                cout<<"reached";
            if(knobs[1]==1){
                //check data hazard between new instruction and rest, implement stalling
                int stalls=0;
                if((checkHazardRS1(IF_DE, DE_EX) || checkHazardRS2(IF_DE, DE_EX)) && IF_DE.isStalled==0){
                    // stalls=3;
                    IF_DE.isStall=3, IF_DE.isStalled=1;
                }
                else if((checkHazardRS1(IF_DE, EX_MA) || checkHazardRS2(IF_DE, EX_MA)) && IF_DE.isStalled==0){
                    // stalls=2;
                    IF_DE.isStall=2, IF_DE.isStalled=1;
                }
                else if((checkHazardRS1(IF_DE, MA_WB) || checkHazardRS2(IF_DE, MA_WB)) && IF_DE.isStalled==0){
                    // stalls=1;
                    IF_DE.isStall=1, IF_DE.isStalled=1;
                }
            }
            else{
                // check data hazard between instructions
                // implement forwarding
                forward(IF_DE, DE_EX, EX_MA, MA_WB);
            }
            if(!MA_WB.isBubble && MA_WB.inst.opcode == 0x7f ){
                updateMem();
                break;
            }

        }
        else{
            IF_DE=fetch(pc,p);
            DE_EX=decode(IF_DE);
            EX_MA=execute(DE_EX, p);
            MA_WB=mem_access(EX_MA);
            writeback(MA_WB);
            pc=p.predict(pc);
        }
    }
    return 0;
}