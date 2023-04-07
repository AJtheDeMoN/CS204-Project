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
int main(int argv, char** argc){
    // Extract command line arguments
    vector<string> args;
    for(int i=2; i<argv; i++){
        args.push_back(argc[i]);
    }
    // Initialize knob flags
    int knobs[5]{};
    //    0                 1             2          3                4
    // pipeline   stalls/forwarding  print_reg   print_pipe_all  print_pipe_x
    for(int i=0; i<args.size(); i++){//setting the knob values
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
    
    // Load instructions from file
    // store_instructions(argc[1]);
    store_instructions("inst.txt");
    // Initialize program counter and clock
    uint32_t pc=0;
    uint32_t clock=0;
    // Initialize pipeline stages
    Pipeline IF_DE, DE_EX, EX_MA, MA_WB;
    Predictor p;
    // Main execution loop
    int NIE=0;//variables intialized to print the required data needed as per the project
    int stalls=0;
    int num_C_inst=0;
    int num_alu=0;
    int num_LS=0;
    int wrong_pred=0;
    while(1){
        if(!knobs[0]){// if pipeline mode is not turned off
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
                uint32_t opcode=DE_EX.instruction&(0x7f);
                if(!DE_EX.isBubble){//if instruction is not a bubble
                    if(opcode==0b1100011 || opcode==0b1101111 || opcode==0b1100111|| opcode==0b0110111|| opcode==0b0010111){
                        num_C_inst++;//counting number of control instructions
                    }
                    else if(opcode==0b0110011 || opcode==0b0010011){
                        num_alu++;//counting number of alu instructions
                    }
                    else if(opcode==0b0000011 || opcode==0b0100011){
                        num_LS++;//counting number of load and store instructions
                    }
                    NIE++;//counting total number of instrutctions
                }
                if(DE_EX.isBubble){
                    stalls++;//counting number of stalls
                }
                IF_DE=fetch(pc, p);
                // update pc
                pc=p.predict(pc);
                if(EX_MA.controls.isBranch && !EX_MA.isBubble && EX_MA.branchTarget!=DE_EX.pc){//if predicted and target value didn't match then stall
                    pc=EX_MA.branchTarget;
                    IF_DE.isBubble=DE_EX.isBubble=true;
                    wrong_pred++;
                }
            }
            clock++;//counting number of cycles
            if(knobs[1]==1){
                //check data hazard between new instruction and rest, implement stalling              
                if((checkHazardRS1(IF_DE, DE_EX) || checkHazardRS2(IF_DE, DE_EX)) && IF_DE.isStalled==0){
                    // stalls=3;
                    IF_DE.isStall=3, IF_DE.isStalled=1;
                    stalls+=3;
                }
                else if((checkHazardRS1(IF_DE, EX_MA) || checkHazardRS2(IF_DE, EX_MA)) && IF_DE.isStalled==0){
                    // stalls=2;
                    IF_DE.isStall=2, IF_DE.isStalled=1;
                    stalls+=2;
                }
                else if((checkHazardRS1(IF_DE, MA_WB) || checkHazardRS2(IF_DE, MA_WB)) && IF_DE.isStalled==0){
                    // stalls=1;
                    IF_DE.isStall=1, IF_DE.isStalled=1;
                    stalls+=1;
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
            NIE++;
            clock+=5;
            IF_DE=fetch(pc,p);
            DE_EX=decode(IF_DE);
            if(DE_EX.inst.opcode==0x7f)
                break;
            EX_MA=execute(DE_EX, p);
            MA_WB=mem_access(EX_MA);
            writeback(MA_WB);
            pc=p.predict(pc);
        }
    }
    cout<<"Number of instruction executed         -> "<<NIE<<"\n";
    cout<<"Total number of cycles                 -> "<<clock<<"\n";
    cout<<"CPI                                    -> "<<(float(clock)/NIE)<<"\n";
    cout<<"Total Number of stalls                 -> "<<stalls<<"\n";
    cout<<"Number of control Instructions         -> "<<num_C_inst<<"\n";
    cout<<"Number of ALU Instructions             -> "<<num_alu<<"\n";
    cout<<"Number of load/store Instructions      -> "<<num_LS<<"\n";
    cout<<"Number of branch mispredictions        -> "<<wrong_pred<<"\n";
    cout<<"Number of data hazards                 -> "<<data_hazard<<"\n";
    cout<<"Number of control hazards              -> "<<wrong_pred<<"\n";
    cout<<"Number of stalls due to data hazards   -> "<<stalls-wrong_pred<<"\n";
    cout<<"Number of stalls due to control hazards-> "<<wrong_pred<<"\n";
    return 0;
}