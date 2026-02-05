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

// Retorna a posição da linha onde o bloco está
int procurarNaCache(Cache *cache, int endBloco) {
    for (int i = 0; i < cache->tamanho; i++) {
        if (cache->linhas[i].valido && cache->linhas[i].endBloco == endBloco) {
            return i;
        }
    }
    return -1; // Não encontrado
}

void atualizarLRU(Cache *cache, int indiceAcessado) {
    for (int i = 0; i < cache->tamanho; i++) {
        if (cache->linhas[i].valido) {
            cache->linhas[i].contador_acesso++;
        }
    }
    
    // reseta quem acabou de ser usado
    if (indiceAcessado != -1) {
        cache->linhas[indiceAcessado].contador_acesso = 0;
    }
}

void escreverBackNaRam(RAM *ram, BlocoMemoria bloco) {
    for (int i = 0; i < TAM_BLOCO; i++) {
        int enderecoReal = (bloco.endBloco * TAM_BLOCO) + i; // (Endereço do Bloco * 4) + Deslocamento
        setDado(ram, enderecoReal, bloco.palavras[i]);
    }
    printf("Write-Back: Bloco %d salvo na RAM.\n", bloco.endBloco);
}

void inserirNaCache(Cache *cache, BlocoMemoria novoBloco, RAM *ram) {
    int indiceSubstituir = encontrarLinhaParaSubstituir(cache);
    BlocoMemoria *linhaVitima = &cache->linhas[indiceSubstituir];

    // salva na RAM os dados da linha vítima
    if (linhaVitima->valido && linhaVitima->atualizado) {
        cache->hits--;
        escreverBackNaRam(ram, *linhaVitima);
    }

    cache->linhas[indiceSubstituir] = novoBloco;
    cache->linhas[indiceSubstituir].atualizado = 0; 
    cache->linhas[indiceSubstituir].contador_acesso = 0; // Zera LRU (é o mais novo)
    cache->linhas[indiceSubstituir].valido = 1;
    cache->linhas[indiceSubstituir].custo = 0; 
}

int encontrarLinhaParaSubstituir(Cache *cache) {
    for (int i = 0; i < cache->tamanho; i++) {
        if (!cache->linhas[i].valido) {
            return i; // Achou um buraco vazio, usa ele
        }
    }

    // Se não tem vazio, procura o "mais velho" 
    int indiceMaisVelho = 0;
    int maiorIdade = -1;

    for (int i = 0; i < cache->tamanho; i++) {
        if (cache->linhas[i].contador_acesso > maiorIdade) {
            maiorIdade = cache->linhas[i].contador_acesso;
            indiceMaisVelho = i;
        }
    }

    return indiceMaisVelho;
}

void buscarNaMemoria(int endereco, int *dadoRetorno, Cache *l1, Cache *l2, Cache *l3, RAM *ram) {
    int endBloco = endereco / TAM_BLOCO;
    int indicePalavra = endereco % TAM_BLOCO;

    BlocoMemoria blocoEncontrado;
    int achou = 0;

    // --- TENTATIVA 1: CACHE L1 ---
    int iL1 = procurarNaCache(l1, endBloco);
    if (iL1 != -1) {
        l1->hits++;
        atualizarLRU(l1, iL1); 
        *dadoRetorno = l1->linhas[iL1].palavras[indicePalavra];
        return; // Sucesso
    } else {
        l1->misses++;
    }

    // --- TENTATIVA 2: CACHE L2 ---
    int iL2 = procurarNaCache(l2, endBloco);
    if (iL2 != -1) {
        l2->hits++;
        atualizarLRU(l2, iL2);
        blocoEncontrado = l2->linhas[iL2]; 
        achou = 1;
    } else {
        l2->misses++;
    }

    // --- TENTATIVA 3: CACHE L3 ---
    if (!achou) {
        int iL3 = procurarNaCache(l3, endBloco);
        if (iL3 != -1) {
            l3->hits++;
            atualizarLRU(l3, iL3);
            blocoEncontrado = l3->linhas[iL3]; 
            achou = 1;
        } else {
            l3->misses++;
        }
    }

    // --- TENTATIVA 4: RAM (Último recurso) ---
    if (!achou) {
        // Monta o bloco pegando 4 inteiros da RAM
        blocoEncontrado.endBloco = endBloco;
        blocoEncontrado.tag = endBloco;
        blocoEncontrado.valido = 1;
        blocoEncontrado.atualizado = 0;
        
        for (int k = 0; k < TAM_BLOCO; k++) {
            int endFisico = (endBloco * TAM_BLOCO) + k;
            blocoEncontrado.palavras[k] = getDado(ram, endFisico);
        }
    }

    // --- CADEIA DE ATUALIZAÇÃO ---
    // Se não estava na L1, tem que ir pra L1.
    // Se veio da RAM, passa por L3 -> L2 -> L1 
    
    if (procurarNaCache(l3, endBloco) == -1) {
        inserirNaCache(l3, blocoEncontrado, ram);
    }
    
    if (procurarNaCache(l2, endBloco) == -1) {
        inserirNaCache(l2, blocoEncontrado, ram);
    }

    inserirNaCache(l1, blocoEncontrado, ram);

    // Retorna o valor para a CPU
    int novoIndice = procurarNaCache(l1, endBloco);
    *dadoRetorno = l1->linhas[novoIndice].palavras[indicePalavra];
}