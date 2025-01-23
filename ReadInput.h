#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <sstream>

#include "instruction.h"

#ifndef TRACE_INPUT_H_
#define TRACE_INPUT_H_

class TraceInput {
  private:
    std::string trace_file_path;
    int start_inst;
    int inst_count;
    int curr_line;
    std::ifstream trace_file;
    
  public:
    // Constructor
    TraceInput(std::string trace_file_path, int start_inst_index, int inst_count);

    bool file_failed;
    
    // Prepares file for reading
    void prepare_file();
    
    // Check if new instruction is needed
    bool is_new_instruction_needed();
    
    // Reads and returns next instruction
    Instruction get_next_instruction();

    // Returns queue with all instructions
    queue<Instruction> getTrace();
    
    // Closes file
    void close_file();
};

#endif
