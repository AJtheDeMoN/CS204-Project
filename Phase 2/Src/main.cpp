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
    store_instructions(argc[1]);

    uint32_t pc=0;
    Pipeline IF_DE, DE_EX, EX_MA, MA_WB;
    Predictor p;
    while(1){
        if(knobs[0]){
            writeback(MA_WB);
            MA_WB=mem_access(EX_MA);
            EX_MA=execute(DE_EX, p);
            DE_EX=decode(IF_DE);
            IF_DE=fetch(pc, p);
            MA_WB=EX_MA;
            EX_MA=DE_EX;
            DE_EX=IF_DE;

            pc=p.predict(pc);
            if(MA_WB.controls.isBranch && MA_WB.branchTarget!=pc)
                pc=MA_WB.branchTarget;
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