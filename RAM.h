#ifndef RAM_H
#define RAM_H
#include <stdlib.h> 


typedef struct {
    int *memoria;
    int tamanho;
} RAM;


void criarRAM(RAM *ram, int tamanho);
void criarRAM_vazia(RAM *ram, int tamanho);
void criarRAM_aleatoria(RAM *ram, int tamanho);
void setDado(RAM *ram, int endereco, int conteudo);
int getDado(RAM *ram, int endereco);
void imprimirRAM(RAM *ram);
void liberarRAM(RAM *ram);

#endif // RAM_H