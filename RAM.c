#include "RAM.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void criarRAM(RAM *ram, int tamanho) {

    if (ram->memoria != NULL) {
        free(ram->memoria);
        ram->memoria = NULL;
    }
    
    // Aloca memoria para o vetor de RAM
    ram->memoria = (int *)calloc(tamanho, sizeof(int));
    if (ram->memoria == NULL) {
        perror("Erro ao alocar memoria para a RAM");
        exit(EXIT_FAILURE);
    }
    ram->tamanho = tamanho;
}

void criarRAM_vazia(RAM *ram, int tamanho) {
    criarRAM(ram, tamanho);
}

void criarRAM_aleatoria(RAM *ram, int tamanho) {

    srand(time(NULL)); 
    criarRAM(ram, tamanho);
    for (int i = 0; i < ram->tamanho; i++) {
        ram->memoria[i] = rand();
    }
}

void setDado(RAM *ram, int endereco, int conteudo) {
    if (endereco >= 0 && endereco < ram->tamanho) {
        ram->memoria[endereco] = conteudo;
    } else {
        printf("Aviso: Tentativa de acesso de endereco invalido na RAM: %d\n", endereco);
    }
}

int getDado(RAM *ram, int endereco) {
    if (endereco >= 0 && endereco < ram->tamanho) { // ram->tamanho é o tamanho alocado
        return ram->memoria[endereco];
    } else {
        
        printf("Erro: Tentativa de acesso de endereco invalido na RAM: %d (Tamanho atual: %d)\n", endereco, ram->tamanho); 
        return 0; 
    }
}

void imprimirRAM(RAM *ram) {
    printf("Conteudo da RAM:\n");
    for (int i = 0; i < ram->tamanho; i++) {
        printf("%d,", ram->memoria[i]);
    }
    printf("\n");
}

void liberarRAM(RAM *ram) {
    if (ram->memoria != NULL) {
        free(ram->memoria);
        ram->memoria = NULL;
        ram->tamanho = 0;
    }
}
