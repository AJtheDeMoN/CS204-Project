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


/* main.cpp 
   Purpose of this file: The file handles the input and output, and
   invokes the simulator
*/


#include "myRISCVSim.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
  char* prog_mem_file; 
  if(argc < 2) {
    printf("Incorrect number of arguments. Please invoke the simulator \n\t./myRISCVSim <input mem file> \n");
    exit(1);
  }
  
  //reset the processor
  reset_proc();
  //load the program memory
  load_program_memory(argv[1]);
  //run the simulator
  run_riscvsim();

  return 1;
}
