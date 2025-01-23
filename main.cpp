#include <iostream>

#include "ReadInput.h"
#include "Simulator.h"

using namespace std;

int main(int argc, char *argv[]){

    if(argc != 5){
        cout<<"Insufficient arguments "<<endl;
        return 0;
    }

    string trace_file_name = argv[1];
    int start_inst = atoi(argv[2]);
    int inst_count = atoi(argv[3]);
    int w = atoi(argv[4]);

    if(start_inst <= 0){
        cout<<"Invalid value of argument "<<endl;
        return 0;
    }
    if(inst_count <= 0){
        cout<<"Invalid value of argument "<<endl;
        return 0;
    }
    if(w <= 0){
        cout<<"Invalid value of argument "<<endl;
        return 0;
    }

    TraceInput trace(trace_file_name, start_inst, inst_count);

    Simulator mySimulator(trace.getTrace(), w);
    mySimulator.simulate();

    return 0;
}
