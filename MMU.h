#ifndef MMU_H
#define MMU_H

#include "RAM.h"

#define TAM_BLOCO 4

// Bloco de memória
typedef struct {
    int palavras[TAM_BLOCO];
    int endBloco;            // Endereço original do bloco na RAM 
    
    // Metadados
    int tag;                 
    int valido;              // 0 = Lixo de memória, 1 = Dado válido
    int atualizado;          // 1 = Foi modificado na cache, precisa salvar na RAM (Dirty bit)
    int contador_acesso;     // LRU (
    int custo;               
    int cacheHit;            
    
} BlocoMemoria;

// A Cache
typedef struct {
    BlocoMemoria *linhas;  
    int tamanho;           
    int id;      
    
    // Estatísticas
    int hits;
    int misses;
} Cache;

void inicializarCache(Cache *cache, int tamanho, int id);
void buscarNaMemoria(int endereco, int *dado, Cache *l1, Cache *l2, Cache *l3, RAM *ram);
void escreverNaMemoria(int endereco, int dado, Cache *l1, Cache *l2, Cache *l3, RAM *ram);
void imprimirEstatisticas(Cache *l1, Cache *l2, Cache *l3);

int procurarNaCache(Cache *cache, int endBloco);
void atualizarLRU(Cache *cache, int indiceAcessado);
int encontrarLinhaParaSubstituir(Cache *cache);
void escreverBackNaRam(RAM *ram, BlocoMemoria bloco);
void inserirNaCache(Cache *cache, BlocoMemoria novoBloco, RAM *ram);

#endif