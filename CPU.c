#include "CPU.h"
#include <stdio.h>
#include <stdlib.h>

void setPrograma(CPU *cpu, Instrucao programaAux[]) {
    
    cpu->programa = programaAux;
}

void iniciarCPU(CPU *cpu, RAM *ram, Cache *l1, Cache *l2, Cache *l3) {
    cpu->opcode = 0;
    cpu->PC = 0;
    
    
    while (cpu->opcode != -1) {
        // Carregar instrucao
        Instrucao inst = cpu->programa[cpu->PC];
        
        // Decodificar instrucao
        cpu->opcode = inst.opcode;
        
        // Executar instrucao
        switch (cpu->opcode) {
            // halt
            case -1: {
                printf("Programa terminou!!\n");
                imprimirRAM(ram);
                break;
            }
            // soma: RAM[add3] = RAM[add1] + RAM[add2]
            case 0: {
                buscarNaMemoria(inst.add1, &cpu->registrador1, l1, l2, l3, ram);
                buscarNaMemoria(inst.add2, &cpu->registrador2, l1, l2, l3, ram);
                cpu->registrador1 += cpu->registrador2;
                // salvar resultado
                escreverNaMemoria(inst.add3, cpu->registrador1, l1, l2, l3, ram);
                printf("Inst sum -> RAM posicao %d com conteudo %d\n", inst.add3, cpu->registrador1);
                break;
            }
            // subtrai: RAM[add3] = RAM[add1] - RAM[add2]
            case 1: {
                buscarNaMemoria(inst.add1, &cpu->registrador1, l1, l2, l3, ram);
                buscarNaMemoria(inst.add2, &cpu->registrador2, l1, l2, l3, ram);
                cpu->registrador1 -= cpu->registrador2;
                // salvar resultado
                escreverNaMemoria(inst.add3, cpu->registrador1, l1, l2, l3, ram);
                printf("Inst sub -> RAM posicao %d com conteudo %d\n", inst.add3, cpu->registrador1);
                break;
            }
            // copia do registrador para RAM: RAM[add2] = Registrador[add1]
            case 2: {
                if (inst.add1 == 1) {
                    escreverNaMemoria(inst.add2, cpu->registrador1, l1, l2, l3, ram);
                    printf("Inst copy_reg_ram -> RAM posicao %d com conteudo %d\n", inst.add2, cpu->registrador1);
                } else if (inst.add1 == 2) {
                    escreverNaMemoria(inst.add2, cpu->registrador2, l1, l2, l3, ram);
                    printf("Inst copy_reg_ram -> RAM posicao %d com conteudo %d\n", inst.add2, cpu->registrador2);
                }
                break;
            }
            // copia da RAM para o registrador: Registrador[add1] = RAM[add2]
            case 3: {
                if (inst.add1 == 1) {
                    buscarNaMemoria(inst.add2, &cpu->registrador1, l1, l2, l3, ram);
                    printf("Inst copy_ram_reg -> Registrador1 com conteudo %d\n", cpu->registrador1);
                } else if (inst.add1 == 2) {
                    buscarNaMemoria(inst.add2, &cpu->registrador2, l1, l2, l3, ram);
                    printf("Inst copy_ram_reg -> Registrador2 com conteudo %d\n", cpu->registrador2);
                }
                break;
            }
            // salva conteudo externo no registrador: Registrador[add1] = add2 (conteudo_externo)
            case 4: {
                if (inst.add1 == 1) {
                    cpu->registrador1 = inst.add2;
                    printf("Inst copy_ext_reg -> Registrador1 com conteudo %d\n", cpu->registrador1);
                } else if (inst.add1 == 2) {
                    cpu->registrador2 = inst.add2;
                    printf("Inst copy_ext_reg -> Registrador2 com conteudo %d\n", cpu->registrador2);
                }
                break;
            }
            // obtem conteudo externo do registrador: inst.add2 = Registrador[add1]
            case 5: {
                
                if (inst.add1 == 1) {
                    cpu->programa[cpu->PC].add2 = cpu->registrador1; // Modificando o array original
                    printf("Inst obtain_reg -> Registrador1 com conteudo %d\n", cpu->registrador1);
                } else if (inst.add1 == 2) {
                    cpu->programa[cpu->PC].add2 = cpu->registrador2; // Modificando o array original
                    printf("Inst obtain_reg -> Registrador2 com conteudo %d\n", cpu->registrador2);
                }
                break;
            }
            case 6: {
                buscarNaMemoria(inst.add1, &cpu->registrador1, l1, l2, l3, ram);
                buscarNaMemoria(inst.add2, &cpu->registrador2, l1, l2, l3, ram);
                cpu->registrador1 = cpu->registrador1%cpu->registrador2;
                // salvar resultado
                escreverNaMemoria(inst.add3, cpu->registrador1, l1, l2, l3, ram);
                printf("Inst rest -> RAM posicao %d com conteudo %d\n", inst.add3, cpu->registrador1);
                break;
            }
            default: {
                printf("Erro: Opcode invalido: %d\n", cpu->opcode);
                cpu->opcode = -1; // Forca halt
                break;
            }
        }
        
        if (cpu->opcode != -1) {
            cpu->PC++;
        }
    }
}