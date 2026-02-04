#ifndef MMU_H
#define MMU_H

// O PDF especifica que cada bloco tem 4 palavras (inteiros)
#define TAM_BLOCO 4

typedef struct {
    int palavras[TAM_BLOCO]; // Os dados propriamente ditos (4 inteiros)
    int endBloco;            // Endereço original do bloco na RAM (útil para Write-Back)
    
    // Metadados para controle da Cache
    int tag;                 // Identificador do bloco (para saber se é o que buscamos)
    int valido;              // 0 = Lixo de memória, 1 = Dado válido
    int atualizado;          // (Dirty Bit) 1 = Foi modificado na cache, precisa salvar na RAM
    int contador_acesso;     // Para a política LRU (Least Recently Used)
    int custo;               // Para contabilizar o custo de acesso (métricas)
    int cacheHit;            // Contador de hits para estatísticas
    
} BlocoMemoria;

// Protótipo da função que vamos implementar depois
// Ela vai substituir o 'getDado' e 'setDado' direto da RAM
void MMU_busca(int endereco, int *dado_retorno);

#endif