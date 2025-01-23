g++ -c instruction.cpp
g++ -c ReadInput.cpp
g++ -c Simulator.cpp
g++ -c main.cpp

g++ instruction.o ReadInput.o Simulator.o main.o -o simulator

del instruction.o
del ReadInput.o
del Simulator.o
del main.o
