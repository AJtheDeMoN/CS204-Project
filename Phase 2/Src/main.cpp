#include <iostream>
#include "./Headers/PC.h"
#include "./Headers/Predictor.h"
#include "./Headers/Pipeline.h"
#include "./Headers/Common.h"
#include "./Headers/Fetch.h"
using namespace std;

int main(int argv, char** argc){
    store_instructions(argc[1]);
    uint32_t pc=-4;
    Pipeline IF_DE, DE_EX, EX_MA, MA_WB;
    Predictor p;
    while(1){
        IF_DE=fetch(pc, p);
        //DE_MX=decode();
        //EX_MA=execute();
        //MA_WB=mem_access();
        //writeback();
        MA_WB=EX_MA;
        EX_MA=DE_EX;
        DE_EX=IF_DE;
    }
    return 0;
}