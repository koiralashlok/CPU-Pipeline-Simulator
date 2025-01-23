#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#include "instruction.h"
#include "ReadInput.h"

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

class Simulator {
  private:
    bool isIntAluIdle, isFloatAluIdle, isBeuIdle,
      isLoadIdle, isStoreIdle, runningBranch;
    unsigned short width;
    vector<Instruction> currentInstructions;
    queue<Instruction> trace;
    int simulatedStats[5];

    // Helper functions
    void freeResource(Instruction);
    int find(string, int);
    bool isInstructionEmpty(Instruction);
    Instruction updateCurrentInstructions();

    // Functions to check if dependencies are satisfied
    bool branchDepSatisfied(Instruction);
    bool aluDepSatisfied(Instruction);
    bool loadStoreDepSatisfied(Instruction);

    // Functions for each stage in the pipeline
    bool newInstr(Instruction*);
    bool fetch(Instruction*);
    bool decode(Instruction*, int);
    bool execute(Instruction*);
    bool memoryAccess(Instruction*);
    bool writeBack(Instruction*);

    // Simulated Statistics Functions
    void updateSimulatedStats(Instruction);
    void printReport(int);
    
  public:
    // Parameterized Constructor
    Simulator(queue<Instruction>, unsigned short);

    // The function that carries out the simulation
    void simulate();

};

#endif
