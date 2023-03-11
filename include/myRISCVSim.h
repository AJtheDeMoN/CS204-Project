/* 

The project is developed as part of Computer Architecture class
Project Name: Functional Simulator for subset of ARM Processor

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


/* myRISCVSim.h
   Purpose of this file: header file for myARMSim
*/

void run_riscvsim();
void reset_proc();
void load_program_memory(char* file_name);
void write_data_memory();
void swi_exit();


//reads from the instruction memory and updates the instruction register
void fetch();
//reads the instruction register, reads operand1, operand2 fromo register file, decides the operation to be performed in execute stage
void decode();
//executes the ALU operation based on ALUop
void execute();
//perform the memory operation
void mem();
//writes the results back to register file
void write_back();


int read_word(char *mem, unsigned int address);
void write_word(char *mem, unsigned int address, unsigned int data);

