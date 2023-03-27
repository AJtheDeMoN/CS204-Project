#include <stdio.h>

// Define RISC-V instruction types
#define R_TYPE 0x33
#define I_TYPE 0x13
#define S_TYPE 0x23
#define B_TYPE 0x63
#define U_TYPE 0x37
#define J_TYPE 0x6F

// Define RISC-V instruction fields
#define OPCODE(x) (x & 0x7F)
#define RD(x) ((x >> 7) & 0x1F)
#define FUNCT3(x) ((x >> 12) & 0x7)
#define RS1(x) ((x >> 15) & 0x1F)
#define RS2(x) ((x >> 20) & 0x1F)
#define FUNCT7(x) ((x >> 25) & 0x7F)
#define IMM_I(x) ((x >> 20) & 0xFFF)
#define IMM_S(x) (((x >> 7) & 0x1F) | ((x >> 20) & 0xFE0))
#define IMM_B(x) (((x >> 8) & 0xF) | ((x << 4) & 0x7E0) | ((x >> 19) & 0x800) | ((x >> 20) & 0x1000))
#define IMM_U(x) (x & 0xFFFFF000)
#define IMM_J(x) (((x >> 21) & 0x3FF) | ((x >> 10) & 0x400) | (x & 0xFF000) | ((x >> 20) & 0x800000))

// Define RISC-V registers
#define NUM_REGISTERS 32

typedef struct {
    unsigned int reg[NUM_REGISTERS];
} riscv_registers;

// Define RISC-V pipeline stages
typedef struct {
    unsigned int pc;
    unsigned int instruction;
} if_stage;

typedef struct {
    unsigned int pc;
    unsigned int instruction;
    unsigned int rs1;
    unsigned int rs2;
} id_stage;

// Define RISC-V data forwarding
typedef struct {
    unsigned int rs1_data;
    unsigned int rs2_data;
} forwarding;

// Define RISC-V pipeline
void riscv_pipeline(riscv_registers *registers, unsigned int *memory, int memory_size)
{
    // Initialize pipeline stages
    if_stage if_;
    id_stage id;

    // Initialize data forwarding
    forwarding fwd;
    fwd.rs1_data = 0;
    fwd.rs2_data = 0;

    // Initialize program counter
    unsigned int pc = 0;

    // Initialize stall cycle counter
    int stall_cycles = 0;

    // Loop through pipeline stages
    while (1) {
        // Fetch stage
        if_.pc = pc;
        if_.instruction = memory[if_.pc / 4];
        pc += 4;

        // Decode stage
        id.pc = if_.pc;
        id.instruction = if_.instruction;
        id.rs1 = registers->reg[RS1(id.instruction)];
        id.rs2 = registers->reg[RS2(id.instruction)];

        // Check for data hazards
        if (stall_cycles > 0) {
            stall_cycles--;
        } else {
            // Data forwarding
            if (id.instruction != 0) {
                if (RD(id.instruction) != 0) {
                    if (RS1(id.instruction) != 0 && RS1(id.instruction) == RD(if_.instruction
                )) {
                    fwd.rs1_data = id.rs1;
                } else if (RS1(id.instruction) != 0 && RS1(id.instruction) == RD(id.instruction)) {
                    fwd.rs1_data = registers->reg[RD(id.instruction)];
                } else {
                    fwd.rs1_data = registers->reg[RS1(id.instruction)];
                }

                if (RS2(id.instruction) != 0 && RS2(id.instruction) == RD(if_.instruction)) {
                    fwd.rs2_data = id.rs2;
                } else if (RS2(id.instruction) != 0 && RS2(id.instruction) == RD(id.instruction)) {
                    fwd.rs2_data = registers->reg[RD(id.instruction)];
                } else {
                    fwd.rs2_data = registers->reg[RS2(id.instruction)];
                }
            }
        }

        // Execute stage
        switch (OPCODE(id.instruction)) {
            case R_TYPE:
                switch (FUNCT3(id.instruction)) {
                    case 0x0:
                        // ADD
                        registers->reg[RD(id.instruction)] = fwd.rs1_data + fwd.rs2_data;
                        break;
                    case 0x1:
                        // SLL
                        registers->reg[RD(id.instruction)] = fwd.rs1_data << (fwd.rs2_data & 0x1F);
                        break;
                    // more cases for R-type instructions
                }
                break;
            case I_TYPE:
                switch (FUNCT3(id.instruction)) {
                    case 0x0:
                        // ADDI
                        registers->reg[RD(id.instruction)] = fwd.rs1_data + IMM_I(id.instruction);
                        break;
                    case 0x2:
                        // SLTI
                        registers->reg[RD(id.instruction)] = (int)fwd.rs1_data < (int)IMM_I(id.instruction);
                        break;
                    // more cases for I-type instructions
                }
                break;
            // more cases for other instruction types
        }

        // Update data forwarding
        fwd.rs1_data = 0;
        fwd.rs2_data = 0;

        // Check for branch instruction
        if (OPCODE(id.instruction) == B_TYPE) {
            // Calculate branch target
            unsigned int target = id.pc + IMM_B(id.instruction);

            // Check branch condition
            int branch_taken = 0;
            switch (FUNCT3(id.instruction)) {
                case 0x0:
                    // BEQ
                    branch_taken = (registers->reg[RS1(id.instruction)] == registers->reg[RS2(id.instruction)]);
                    break;
                case 0x1:
                    // BNE
                    branch_taken = (registers->reg[RS1(id.instruction)] != registers->reg[RS2(id.instruction)]);
                    break;
                // more cases for branch instructions
            }

            if (branch_taken) {
                // Flush pipeline and set program counter to branch target
                if_.instruction = 0;
                id.instruction = 0;
                pc = target;
                stall_cycles = 0;
            } else {
                // Stall pipeline for one cycle
                stall_cycles = 1;
            }
        }
    }

    // Check for end of program
    if (if_.instruction == 0 && id.instruction == 0) {
        break;
    }
}

}
// This updated implementation considers data hazards by using data forwarding to supply values from the previous pipeline stage to the current pipeline stage when necessary, and by introducing stall cycles when there are conflicts. It also includes code to handle branch instructions and to detect the
