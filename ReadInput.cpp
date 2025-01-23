#include "ReadInput.h"

TraceInput::TraceInput(string trace_file_name, int start_inst, int inst_count) {
    this->trace_file_path = trace_file_name;
    this->start_inst = start_inst;
    this->inst_count = inst_count;
    curr_line = 1;
    file_failed = false;
    prepare_file();
}

void TraceInput::prepare_file() {
    string line;
    trace_file.open(trace_file_path);
    if (trace_file.is_open()) {
        // skip lines until start_inst is reached
        int i;
        for (i = 1; i < start_inst; i++) {
            getline(trace_file, line);
            if (trace_file.eof()) {
                file_failed = true;
                return;
            }
        }

        // Shlok Koirala: Set curr_line
        curr_line = i;
        
    } else {
        file_failed = true;
    }
}

// This function checks whether a new instruction is needed or not
bool TraceInput::is_new_instruction_needed() {
    return curr_line < start_inst + inst_count;
}


// This function reads the next line of the trace file and returns an Instruction object
Instruction TraceInput::get_next_instruction() {
    if (!trace_file.is_open() || trace_file.eof()) {
        cerr << "Error: file not open or end of file reached\n";
        exit(1);
    }

    string line;
    getline(trace_file, line);
    curr_line++;

    vector<string> tokens;
    stringstream ss(line);
    string token;
    while (getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    if (tokens.size() < 2) {
        cerr << "Error: invalid line format\n";
        exit(1);
    }

    Instruction instruction;
    instruction.program_counter = tokens[0];

    // Shlok Koirala: added -1 to match enum indexing
    int type = stoi(tokens[1]) - 1;
    instruction.type = static_cast<InstructionType>(type);

    
    for (int i = 2; i < static_cast<int>(tokens.size()); i++) {
        instruction.addDependency(tokens[i]);
    }

    return instruction;
}

// This function closes the trace file
void TraceInput::close_file() {
    trace_file.close();
}

/**
 * Author: Shlok Koirala
 * Generate overall trace to be used by Simulator
 * @returns trace queue with all instructions
*/
queue<Instruction> TraceInput::getTrace(){

    queue<Instruction> trace;

    while(is_new_instruction_needed())
        trace.push(get_next_instruction());

    close_file();
    
    return trace;
}