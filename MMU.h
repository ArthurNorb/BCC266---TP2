#ifndef MMU_H
#define MMU_H

#define TAM_BLOCO 4

typedef struct {
    int palavras[TAM_BLOCO];
    int endBloco;            // Endereço original do bloco na RAM 
    
    // Metadados para controle da Cache
    int tag;                 
    int valido;              // 0 = Lixo de memória, 1 = Dado válido
    int atualizado;          // 1 = Foi modificado na cache, precisa salvar na RAM
    int contador_acesso;     // LRU (
    int custo;               
    int cacheHit;            
    
} BlocoMemoria;

void MMU_busca(int endereco, int *dado_retorno);

#endif