#include <string>
#include <vector>

using namespace std;

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

enum class InstructionType {
INTEGER,
FLOATING_POINT,
BRANCH,
LOAD,
STORE
};

// Shlok Koirala: added to track instruction stage (interchangably used with "phase" in comments)
enum class InstructionStage {
NW, // New
IF, // Fetch
DE, // Decode
EX, // Execute
MM, // Memory
WB, // Write Back
RT  // Retired
};

class Instruction {
public:

    Instruction();
    
    // Parameterized constructor
    Instruction(string pc, InstructionType type, vector<string> deps);
    
    // Member variables
    string program_counter;
    InstructionType type;
    InstructionStage currentStage;
    vector<string> dependencies;

    // Method to add a dependency
    void addDependency(string dep);

};

#endif
