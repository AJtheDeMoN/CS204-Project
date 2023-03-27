#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class IF;
class ID;
class EX;
class MEM;
class WB;

class Instruction {
public:
    uint32_t opcode : 7;
    uint32_t rd : 5;
    uint32_t funct3 : 3;
    uint32_t rs1 : 5;
    uint32_t rs2 : 5;
    uint32_t funct7 : 7;
    int32_t imm12 : 12;
};

// Pipeline registers and properties
class PipelineRegister {
    public:
    Instruction instruction;
    uint32_t alu_op1;
    uint32_t alu_op2;
    uint32_t alu_result;
    uint32_t pc;
    PipelineRegister() {
        instruction.opcode = 0;
        instruction.rd = 0;
        instruction.funct3 = 0;
        instruction.rs1 = 0;
        instruction.rs2 = 0;
        instruction.funct7 = 0;
        instruction.imm12 = 0;
        alu_op1 = 0;
        alu_op2 = 0;
        alu_result = 0;
        pc = 0;
    }
};

class PipelineProperties {
    public:
    bool id_stall;
    bool ex_stall;
    bool mem_stall;
    bool is_branch;
    bool is_jump;
    bool is_jalr;
    int32_t imm_b;
    int32_t imm_j;
    int32_t imm_jl;
    PipelineProperties() {
        id_stall = false;
        ex_stall = false;
        mem_stall = false;
        is_branch = false;
        is_jump = false;
        is_jalr = false;
        imm_b = 0;
        imm_j = 0;
        imm_jl = 0;
    }
};

PipelineRegister if_id; // Fetch-Decode
PipelineRegister id_ex; // Decode-Execute
PipelineRegister ex_mem; // Execute-Memory
PipelineRegister mem_wb; // Memory-Writeback

PipelineProperties pipeline_props; // Pipeline properties

class IF {
public:
    uint32_t pc;
    uint32_t next_pc;

    void execute(uint8_t* memory, Instruction& if_id) {
        if_id = *((Instruction*)&memory[pc]);
        next_pc=pc+4;
    }
};

class ID {
public:
    Instruction if_id;
    Instruction id_ex;
    uint32_t ex_pc;

    void execute(uint32_t* registers, ID& id, EX& ex, MEM& mem, IF& iff) {
        id_ex = if_id;
        ex_pc=iff.next_pc;

        // Detect data hazards and handle them using forwarding and stalling
        if (id_ex.rs1 == ex.id_ex.rd) {
            ex.forward_alu1 = ex.alu_result;
        }
        if (id_ex.rs1 == mem.id_ex.rd) {
            mem.forward_alu1 = mem.alu_result;
        }
        if (id_ex.rs2 == ex.id_ex.rd) {
            ex.forward_alu2 = ex.alu_result;
        }
        if (id_ex.rs2 == mem.id_ex.rd) {
            mem.forward_alu2 = mem.alu_result;
        }

        // Check for branch and jump instructions and update pipeline properties
        if (id_ex.opcode == 0b1100011) { // BEQ/BNE/BLT/BGE
            pipeline_props.is_branch = true;
            pipeline_props.imm_b = (id_ex.imm12 & 0x1) << 11 | (id_ex.imm12 & 0x3fe) >> 1;
        }
        else if (id_ex.opcode == 0b1101111) { // JAL
            pipeline_props.is_jump = 1;
            pipeline_props.imm_j = ((ex_pc + id_ex.imm12) & 0xfffff000) | (id_ex.imm12 & 0xfff);
        }
        else if (id_ex.opcode == 0b1100111) { // JALR
            pipeline_props.is_jump = 2;
            pipeline_props.imm_jl = id_ex.imm12;
        }


        // if (ex.stall) {
        //     id_stall = 1;
        //     id_ex = id.id_ex;
        // }
        // else {
        //     id_stall = 0;
        //     id_ex = if_id;
        // }
    }
};

class EX {
public:
    Instruction id_ex;
    uint32_t forward_alu1;
    uint32_t forward_alu2;
    uint32_t alu_result;
    uint8_t branch;
    uint8_t stall;

    void execute(uint32_t* registers, EX& ex, MEM& mem, WB& wb) {
        // Perform arithmetic or logic operations
        switch (id_ex.opcode) {
        case 0b0110011: // R-type instructions
            switch (id_ex.funct3) {
            case 0b000:
                if (id_ex.funct7 == 0b0000000) { // ADD
                    alu_result = forward_alu1 + forward_alu2;
                }
                else if (id_ex.funct7 == 0b0100000) { // SUB
                    alu_result = forward_alu1 - forward_alu2;
                }
                break;
            case 0b001: // SLL
                alu_result = forward_alu1 << (forward_alu2 & 0x1f);
                break;
            // case 0b010: // SLT
            // to be continued

               
            }
        }
    }
};

class MEM {
public:
    Instruction id_ex;
    uint32_t forward_alu2;
    uint32_t forward_alu1;
    uint8_t* memory;
    uint32_t alu_result;
    uint8_t write_memory;
    uint8_t read_memory;
    uint8_t stall;
    uint8_t id_stall;



    void execute(EX& ex, MEM& mem, WB& wb) {
    if (write_memory) {
        *((uint32_t*)&memory[alu_result]) = wb.wb_data;
    }
    else if (read_memory) {
        alu_result = *((uint32_t*)&memory[alu_result]);
    }
    else {
        alu_result = ex.alu_result;
    }
}
};

class WB {
public:
Instruction id_ex;
uint32_t alu_result;
uint32_t mem_result;
uint32_t wb_data;
void execute(uint32_t* registers) {
    switch (id_ex.opcode) {
    case 0b0110011: // R-type instructions
        registers[id_ex.rd] = alu_result;
        break;
    // case 0b0010011: // I-type instructions
    // case 0b0000011: // Load instructions
    // case 0b0100011: // Store instructions
    // case 0b1100011: // Branch instructions
    // case 0b0010111: // AUIPC
    // case 0b0110111: // LUI
    // case 0b1101111: // JAL
    // case 0b1100111: // JALR
    }
}
};


int main() {
uint8_t memory[100000]; // 100 KB memory
uint32_t registers[32]; // 32 registers
IF iff;
ID id;
EX ex;
MEM mem;
WB wb;
// Load program to memory
ifstream file;
file.open("program.bin", ios::in | ios::binary);
if (file.is_open()) {
    file.read((char*)memory, sizeof(memory));
    file.close();
}

while (true) {
    // Writeback stage
    wb.execute(registers);

    // Memory stage
    mem.execute(ex, mem, wb);

    // Execute stage
    ex.execute(registers, ex, mem, wb);

    // Decode stage
    id.execute(registers, id, ex, mem, iff);

    // Fetch stage
    // iff.execute(memory, if_id);
}

return 0;
}