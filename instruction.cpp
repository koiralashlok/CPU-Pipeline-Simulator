#include "instruction.h"
#include <iostream>

/**
 * Default Constructor
 * Creates an empty program_counter with type "empty"
*/
Instruction::Instruction() {
    program_counter = "empty";
    type = InstructionType::INTEGER;
    dependencies = {};
    currentStage = static_cast<InstructionStage>(0);    // default to "new" type
}

// Parameterized constructor
Instruction::Instruction(string pc, InstructionType t, vector<string> deps) {
    program_counter = pc;
    type = t;
    dependencies = deps;
    currentStage = static_cast<InstructionStage>(0);    // default to "new" type
}

// Method to add a dependency
void Instruction::addDependency(string dep) {
    dependencies.push_back(dep);
}
