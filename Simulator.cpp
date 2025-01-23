# include "Simulator.h"
# include "instruction.h"


/**
 * Frees resource occupied by I
 * @param I Instruction whose resource is to be freed
*/
void Simulator::freeResource(Instruction I){

    switch(I.type){
        
        case static_cast<InstructionType>(0):
            isIntAluIdle = 1;
            return;
        case static_cast<InstructionType>(1):
            isFloatAluIdle = 1;
            return;
        case static_cast<InstructionType>(2):
            isBeuIdle = 1;
            runningBranch = 0;
            return;
        case static_cast<InstructionType>(3):
            isLoadIdle = 1;
            return;
        case static_cast<InstructionType>(4):
            isStoreIdle = 1;
            return;
    }
}

/**
 * Parameterized Constructor
*/
Simulator::Simulator(queue<Instruction> trace, unsigned short width){
    this->trace = trace; 
    this->width = width;

    isIntAluIdle = isFloatAluIdle = isBeuIdle = isLoadIdle = isStoreIdle = 1;
    runningBranch = 0;

    currentInstructions = vector<Instruction>();

    simulatedStats[0] = simulatedStats[1] = simulatedStats[2] = simulatedStats[3] = simulatedStats[4] = 0; 
}

/**
 * Helper to decide if instruction is empty
 * @param I instruction to check
 * @returns true if I is empty, false otherwise
*/
bool Simulator::isInstructionEmpty(Instruction I){
    return I.program_counter == "empty";
}

/**
 * Rotate currentInstructions anti-clockwise, first element deleted; Every element moves left by 1
 * New instruction is now in the last position; If no more instructions, size reduces by 1
 * @returns deleted instruction, if currentInstructions is empty returns empty Instruction
*/
Instruction Simulator::updateCurrentInstructions(){

    Instruction I;
    int n = (int)currentInstructions.size();

    if(n != 0)
        I = currentInstructions.front();

    // Always remove top except when populating for the first time, i.e.:
    //  If trace is not empty and max # instr in pipeline
    //  Or if trace empty (start depopulating)
    if((n-- == width * 5 && !trace.empty()) || trace.empty()){

        // n already reduced by 1 so iteration is within bounds
        for(int i = 0; i < n; i++)
            currentInstructions.at(i) = currentInstructions.at(i + 1);

        currentInstructions.erase(currentInstructions.begin() + n);
    }

    if(!trace.empty()){
        currentInstructions.push_back(trace.front());
        trace.pop();
    }

    return I;
}

/**
 * Finds instruction corresponding to pc among currentInstructions
 * @param pc program_counter to find
 * @returns index of Instruction that has "program_counter" == pc in currentInstructions
*/
int Simulator::find(string pc, int start){

    for(int i = start - 1; i >= 0; i--){
        if(pc == currentInstructions.at(i).program_counter)
            return i;
    }

    // dependency either already completed or was from older trace
    return -1;
}

/**
 * Helper function to decide if a depdendency on branch instruction I is satisfied
 * @param I dependence
 * @return true if dependence satisfied (I has completed EX phase), false otherwise
*/
bool Simulator::branchDepSatisfied(Instruction I){

    // Current stage must not be New - EX, anything after/else is fine
    return I.currentStage != static_cast<InstructionStage>(0) && 
        I.currentStage != static_cast<InstructionStage>(1) &&
        I.currentStage != static_cast<InstructionStage>(2) &&
        I.currentStage != static_cast<InstructionStage>(3);
}

/**
 * Helper function to decide if a depdendency on alu (int/ float OP) instruction I is satisfied
 * @param I dependence
 * @return true if dependence satisfied (I has completed EX phase), false otherwise
*/
bool Simulator::aluDepSatisfied(Instruction I){

    // Current stage must not be New - EX, anything after/else is fine
    return branchDepSatisfied(I);
}

/**
 * Helper function to decide if a depdendency on load/ store instruction I is satisfied
 * @param I dependence
 * @return true if dependence satisfied (I has completed MEM phase), false otherwise
*/
bool Simulator::loadStoreDepSatisfied(Instruction I){

    // Current stage must not be New - MEM, anything after/else is fine
    return branchDepSatisfied(I) && I.type != static_cast<InstructionType>(4);
}

bool Simulator::newInstr(Instruction *I){

    // If a branch is being run (not completed exec phase), I must wait to be fetched
    if(runningBranch)
        return 1;

    // If I is a branch Instruction
    if((*I).type == static_cast<InstructionType>(2))
        runningBranch = 1;

    // Either no dependency exists or all dependencies satisfied, can move to next phase
    // currentStage of I is now IF
    (*I).currentStage = static_cast<InstructionStage>(1);

    // Also do fetch since "new" is not really a stage
    return fetch(I);
}

/**
 * Carry out fetch (IF) stage of CPU pipeline
 * @param I instruction to be fetched
 * @return true if stalled, else false
*/
bool Simulator::fetch(Instruction *I){

    // currentStage of I is now DE
    (*I).currentStage = static_cast<InstructionStage>(2);

    return 0;
}

/**
 * Carry out decode (DE) stage of CPU pipeline
 * @param I instruction to be fetched
 * @return true if stalled, else false
*/
bool Simulator::decode(Instruction *I, int ind){

    // If data dependency exists stall (stay in decode, don't proceed to execute)
    //  dependency on ALU satisfied after EX phase is completed
    //  dependency on load/store after MEM phase is completed
    // Note: ALU dependency here is a data dependency, once instruction using ALU finishes EX the result can be used
    for (int i = 0; i < (int)(*I).dependencies.size(); i++){

        int index = find((*I).dependencies.at(i), ind);

        if(index == -1)
            continue;

        Instruction dependency;

        // If dependency instruction is int/ float OP and dependency not satisfied then return
        if((dependency.type == static_cast<InstructionType>(0) || dependency.type == static_cast<InstructionType>(1)) && !aluDepSatisfied(dependency))
            return 1;

        // If dependency instruction is load/ store OP and dependency not satisfied then return
        if((dependency.type == static_cast<InstructionType>(3) || dependency.type == static_cast<InstructionType>(4)) && !loadStoreDepSatisfied(dependency))
            return 1;
    }

    switch((*I).type){

        // If int/ float ALU needed but busy, stall till the dependency completes EX phase

        // If int OP and ALU busy, return
        case static_cast<InstructionType>(0):
        if(!isIntAluIdle)
            return 1;
        // Else, occupy the int ALU
        isIntAluIdle = 0;
        break;

        // If float OP and ALU busy, return
        case static_cast<InstructionType>(1):
        if(!isFloatAluIdle)
            return 1;
        // Else, occupy the float ALU
        isFloatAluIdle = 0;
        break;

        // If BEU needed but busy, stall then stall till the dependency completes EX phase

        // If branch and BEU busy, return
        case static_cast<InstructionType>(2):
        if(!isBeuIdle)
            return 1;
        // Else, occupy the BEU
        isBeuIdle = 0;
        runningBranch = 1;
        break;

        default:
            break;
    }

    // Either no dependency exists or all dependencies satisfied, can move to next phase
    // Current Stage of I is now EX
    (*I).currentStage = static_cast<InstructionStage>(3);
    return 0;
}

/**
 * Carry out execute (EX) stage of CPU pipeline
 * @param I instruction to be fetched
 * @return true if stalled, else false
*/
bool Simulator::execute(Instruction *I){

    // Note: ALU dependency here is structural, cannot use ALUs if they are already in use
    switch((*I).type){

        // If inst is LOAD and another LOAD is in MEM phase, stall
        // If inst is STORE and another STORE is in MEM phase, stall
        // Freed once MEM phase is complete not here

        // If load and there is a load in MEM phase, return
        case static_cast<InstructionType>(3):
        if(!isLoadIdle)
            return 1;
        // Else, start load
        isLoadIdle = 0;
        break;

        // If store and there is a store in MEM phase, return
        case static_cast<InstructionType>(4):
        if(!isStoreIdle)
            return 1;
        // Else, start store
        isStoreIdle = 0;
        break;

        default:
            break;
    }

    if((int)(*I).type == 0 || (int)(*I).type == 1 || (int)(*I).type == 2)
        freeResource(*I);

    // Either no dependency exists or all dependencies satisfied, can move to next phase
    // Current Stage of I is now MEM
    (*I).currentStage = static_cast<InstructionStage>(4);
    return 0;
}

/**
 * Carry out memory access (MEM) stage of CPU pipeline
 * @param I instruction to be fetched
 * @return true if stalled, else false
*/
bool Simulator::memoryAccess(Instruction *I){

    // Free up the resource(s) that I was using
    if((int)(*I).type == 3 || (int)(*I).type == 4)
        freeResource(*I);

    // Current Stage of I is now WB
    (*I).currentStage = static_cast<InstructionStage>(5);
    return 0;
}

/**
 * Carry out write back (WB) stage of CPU pipeline
 * @param I instruction to be fetched
 * @return true if stalled, else false
*/
bool Simulator::writeBack(Instruction *I){

    // Current Stage of I is now retired
    (*I).currentStage = static_cast<InstructionStage>(6);
    return 0;
}

/**
 * Increment corresponding Instruction type processed count in simulatedStats
 * @param I Instruction whose type is to be incremented
*/
void Simulator::updateSimulatedStats(Instruction I){

    switch(I.type){
        
        case static_cast<InstructionType>(0):
            simulatedStats[0] += 1;
            return;
        case static_cast<InstructionType>(1):
            simulatedStats[1] += 1;
            return;
        case static_cast<InstructionType>(2):
            simulatedStats[2] += 1;
            return;
        case static_cast<InstructionType>(3):
            simulatedStats[3] += 1;
            return;
        case static_cast<InstructionType>(4):
            simulatedStats[4] += 1;
            return;
    }
}

void Simulator::printReport(int clock){

    double tot = (simulatedStats[0] + simulatedStats[1] + simulatedStats[2] + simulatedStats[3] + simulatedStats[4])
        / 100.0;

    double integer = simulatedStats[0]/tot, floating = simulatedStats[1]/tot, branch = simulatedStats[2]/tot,
        load = simulatedStats[3]/tot, store = simulatedStats[4]/tot;

    cout << "Clock Cycles elapsed:\t\t" << clock << endl;

    cout << "Integer instructions:\t\t";
    printf("%.4f", integer);
    cout << "%\n";

    cout << "Floating point instructions:\t";
    printf("%.4f", floating);
    cout << "%\n";

    cout << "Branch instructions:\t\t";
    printf("%.4f", branch);
    cout << "%\n";

    cout << "Load instructions:\t\t";
    printf("%.4f", load);
    cout << "%\n";

    cout << "Store instructions:\t\t";
    printf("%.4f", store);
    cout << "%\n\n";
}

/**
 * Create a simulation using the parameters provided to the constructor
*/
void Simulator::simulate(){

    cout << "Starting Simulation...\n\n";

    int clock = 1;
    bool stalled = 0;

    // Retrieve first instruction(s)
    for(int ii = 0; ii < width; ii++)
        updateCurrentInstructions();

    Instruction *I;

    // Simulation Loop
    // just loop thru and check what the current instruction stage is and do that
    // if an instruction is stalled nothing after it can be done so if is stalled, update params and go to next iteration
    // free resources when? what about when stall break occurs?
    while(true){

        int i;
        for (i = 0; i < (int)currentInstructions.size(); i++){
            
            I = &(currentInstructions.at(i));

            switch((*I).currentStage){

                // New
                case static_cast<InstructionStage>(0):
                    stalled = newInstr(I);
                    break;

                // Fetch
                case static_cast<InstructionStage>(1):
                    stalled = fetch(I);
                    break;

                // Decode    
                case static_cast<InstructionStage>(2):
                    stalled = decode(I, i);
                    break;

                // Execute
                case static_cast<InstructionStage>(3):
                    stalled = execute(I);
                    break;

                // Memory Access
                case static_cast<InstructionStage>(4):
                    stalled = memoryAccess(I);
                    break;

                // Write Back
                case static_cast<InstructionStage>(5):
                    stalled = writeBack(I);
                    break;
                
                default:
                    break;
            }

            if(stalled)
                break;
                
        }

        // only instructions from the first w will be retired (0 to w-1)
        for(int ii = 0; ii < min(width, (unsigned short)currentInstructions.size()); ii++){

            bool retiredFlag = currentInstructions.at(0).currentStage == static_cast<InstructionStage>(6);

            // update when either not full size or instr retired
            // .at(0) since after updating the removed instruction is gone and next to be checked is at the front
            if(retiredFlag || (currentInstructions.size() < width * 5 && !trace.empty())){

                Instruction retired = updateCurrentInstructions();

                if(retiredFlag)
                    updateSimulatedStats(retired);
            }
        }

        if(clock % 200000 == 0)
            printReport(clock);

        // Reset stalled
        stalled = 0;

        // Update number of clock cycles elapsed
        if(i != 0)
            clock++;

        // If currentInstructions is empty (no more instructions were retrieved from trace), end of sim
        if(currentInstructions.empty())
            break;
    }

    cout << "Simulation Results" << endl;
    printReport(clock);
    cout << "Done exiting...\n";
}
