#ifndef CPU_H
#define CPU_H

#include "Instrucao.h"
#include "RAM.h"

typedef struct {
    int registrador1;
    int registrador2;
    int PC; // Program Counter
    Instrucao *programa; // Vetor de instrucoes
    int opcode;
} CPU;

void setPrograma(CPU *cpu, Instrucao programaAux[]);
void iniciarCPU(CPU *cpu, RAM *ram);

#endif // CPU_H
