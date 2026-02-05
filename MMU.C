#include "MMU.h"
#include <stdio.h>
#include <stdlib.h>

void inicializarCache(Cache *cache, int tamanho, int id) {
    cache->id = id;
    cache->tamanho = tamanho;
    cache->hits = 0;
    cache->misses = 0;

    cache->linhas = (BlocoMemoria *) malloc(tamanho * sizeof(BlocoMemoria));
    
    if (cache->linhas == NULL) {
        printf("Erro fatal: Sem memória para alocar Cache L%d!\n", id);
        exit(1);
    }

    for (int i = 0; i < tamanho; i++) {
        cache->linhas[i].valido = 0;
        cache->linhas[i].tag = -1;
        cache->linhas[i].atualizado = 0;     // Dirty bit limpo
        cache->linhas[i].contador_acesso = 0; // LRU zerado
        cache->linhas[i].endBloco = -1;
        
        for(int j=0; j<TAM_BLOCO; j++){
            cache->linhas[i].palavras[j] = 0;
        }
    }
    
    printf("Cache L%d inicializada com %d linhas.\n", id, tamanho);
}

void imprimirEstatisticas(Cache *l1, Cache *l2, Cache *l3) {
    printf("\n=== RESUMO DE ESTATISTICAS ===\n");
    printf("Cache L1 -> Hits: %d | Misses: %d\n", l1->hits, l1->misses);
    printf("Cache L2 -> Hits: %d | Misses: %d\n", l2->hits, l2->misses);
    printf("Cache L3 -> Hits: %d | Misses: %d\n", l3->hits, l3->misses);
}