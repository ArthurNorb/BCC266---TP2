#include "CPU.h"
#include "RAM.h"
#include "Instrucao.h"
#include "MMU.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Funcoes de operacoes
void programaAleatorio(RAM *ram, CPU *cpu, int qdeIntrucoes);
void programaSomaMatriz(RAM *ram, CPU *cpu, int cardinalidade);
void programaFat(RAM *ram, CPU *cpu, int fat);
void programaDiv(RAM *ram, CPU *cpu, int dividendo, int divisor);
void programaMult(RAM *ram, CPU *cpu, int multiplicando, int multiplicador);
void programaEuclides(RAM *ram, CPU *cpu, int a, int b);
void programaMMC(RAM *ram, CPU *cpu, int a, int b);
void programaDet(RAM *ram, CPU *cpu, int a00, int a01, int a10, int a11);
void programaExp(RAM *ram, CPU *cpu, int base, int expoente);
void programaFibonat(RAM *ram, CPU *cpu, int n);
void programaPrimo(RAM *ram, CPU *cpu, int n);
void programaRaizQuadrada(RAM *ram, CPU *cpu, int n);
void programaRaizCubica(RAM *ram, CPU *cpu, int n);

void limpar_buffer() {
   int ch;
   while ((ch = getchar()) != '\n' && ch != EOF);
}

void limpar_tela() {
    #ifdef _WIN32
        // Comando para limpar a tela no Windows
        system("cls");
    #else
        // Comando para limpar a tela em sistemas Unix (Linux, macOS, etc.)
        system("clear");
    #endif
}

void imprimeMenu(){

    limpar_tela();
        printf("=========================\n");
        printf("Bem-vindo a nossa maquina\n");
        printf("=========================\n\n");
        printf("[1]  Programa Aleatorio\n");
        printf("[2]  Programa Multiplicacao\n");
        printf("[3]  Programa Divisao\n");
        printf("[4]  Programa Fatoracao\n");
        printf("[5]  Programa Euclides\n");
        printf("[6]  Programa MMC\n");
        printf("[7]  Programa Determinante 2x2\n");
        printf("[8]  Programa Exponencial\n");
        printf("[9]  Programa Fibonat\n");
        printf("[10] Programa Numeros Primos\n");
        printf("[11] Programa Raiz Quadrada\n");
        printf("[12] Programa Raiz Cubica\n");
        printf("[13] Rodar Arquivo 'instructions2.txt'\n");
        printf("[0] Sair\n");
        printf("Digite um dos nuneros para escolher uma opcao: ");
}


// Funcao para alocar um programa
Instrucao* alocarPrograma(int tamanho) {

    Instrucao *programa = (Instrucao*)malloc(tamanho * sizeof(Instrucao));
    if (programa == NULL) {
        printf("\nErro na alocacao do programa\n");
        exit(EXIT_FAILURE);
    }
    return programa;
}


void programaAleatorio(RAM *ram, CPU *cpu, int qdeIntrucoes) {

    Instrucao *umPrograma = alocarPrograma(qdeIntrucoes);
    int tamanhoRAM = 1000;
    
    criarRAM_aleatoria(ram, tamanhoRAM);

    // configuração: L1=8 blocos, L2=16 blocos, L3=32 blocos
    Cache l1, l2, l3;
    inicializarCache(&l1, 8, 1);
    inicializarCache(&l2, 16, 2);
    inicializarCache(&l3, 32, 3);
    
    for (int i = 0; i < (qdeIntrucoes - 1); i++) {
        umPrograma[i].opcode = rand() % 2; // 0 ou 1
        umPrograma[i].add1 = rand() % tamanhoRAM;
        umPrograma[i].add2 = rand() % tamanhoRAM;
        umPrograma[i].add3 = rand() % tamanhoRAM;
    }
    
    umPrograma[qdeIntrucoes - 1].opcode = -1;
    
    setPrograma(cpu, umPrograma);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    imprimirEstatisticas(&l1, &l2, &l3);

    free(l1.linhas);
    free(l2.linhas);
    free(l3.linhas);
    free(umPrograma);
}
// Funcao de multiplicacao
void programaMult(RAM *ram, CPU *cpu, int multiplicando, int multiplicador) {
    printf("\n\t\t\tExecutando programaMult (%d x %d)\n", multiplicando, multiplicador);
    criarRAM_vazia(ram, 2);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);
    
    Instrucao trecho1[] = {
        {4, 1, multiplicando, 0}, // opcode 4: salva multiplicando no registrador 1
        {2, 1, 1, 0},             // opcode 2: copia reg 1 para RAM[1]
        {-1, 0, 0, 0}             // opcode -1: halt
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    // Loop de soma (multiplicador vezes)
    for (int i = 0; i < multiplicador; i++) {
        Instrucao trecho2[] = {
            {0, 0, 1, 0}, // opcode 0: soma RAM[0] + RAM[1] -> RAM[0]
            {-1, 0, 0, 0} // opcode -1: halt
        };
        setPrograma(cpu, trecho2);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
    }
    
    // Trecho 3: Le o resultado (RAM[0]) para o registrador e ganha o valor (opcode 5)
    Instrucao trecho3[] = {
        {3, 1, 0, 0},    //opcode 3: copia RAM[0] para reg 1
        {5, 1, -1, -1}, // opcode 5: obtem conteudo do reg 1, colocando em trecho3[1].add2
        {-1, 0, 0, 0}  //  opcode -1: halt
    };
    setPrograma(cpu, trecho3);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    printf("\nO resultado da multiplicacao eh: %d\n", trecho3[1].add2);
}

// Funcao de Divisao
void programaDiv(RAM *ram, CPU *cpu, int dividendo, int divisor) {

    printf("\n\t\t\tExecutando programaDiv (%d / %d)\n", dividendo, divisor);

    criarRAM_vazia(ram, 4);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    // Trecho 1: Inicializa RAM[0] com o dividendo e a RAM[1] com o divisor
    Instrucao trecho1[] = {
        {4, 1, dividendo, 0},  // Reg 1 = dividendo
        {2, 1, 0, 0},         //  RAM[0] = Reg 1
        {4, 2, divisor, 0},  //   Reg 2 = divisor
        {2, 2, 1, 0},       //    RAM[1] = Reg 2
        {-1, 0, 0, 0}      //     opcode -1: halt
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    // Trecho 2: Inicializa RAM[2] para ser o contador
    Instrucao trecho2[] = {
        {4, 1, 1, 0},         // Reg 1 = 1
        {2, 1, 2, 0},        //  RAM[2] = 1 (contador)
        {-1, 0, 0, 0}	    //   opcode -1: halt
    };
    setPrograma(cpu, trecho2);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    int dividendoAtual = dividendo;
    while (dividendoAtual >= divisor) {
    
        Instrucao trecho3[] = {
            {1, 0, 1, 0},        // opcode 1 : Sub: RAM[0] = RAM[0] - RAM[1]
            {0, 3, 2, 3},       //  Soma: RAM[3] = RAM[3] + RAM[2]
            {3, 1, 0, 0},      //   Opcode 3 :Copia: Reg 1 = RAM[0]
            {5, 1, -1, -1},   //    Opcode 5 : Obtem: reg 1 para trecho3[3].add2
            {-1, 0, 0, 0}    //     Opcode -1: halt
        };
        setPrograma(cpu, trecho3);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        
        dividendoAtual = trecho3[3].add2; 
    }

    
    Instrucao trecho4[] = {
        {3, 1, 3, 0},       // Opcode 3 : Reg 1 = RAM [3]
        {5, 1, -1, -1},    //  Opcode 5 : Ganha Reg 1 para trecho4[1].add2
        {-1, 0, 0, 0}     //   Opcode -1: halt
    };
    setPrograma(cpu, trecho4);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    printf("\nO resultado da divisao eh: %d\n", trecho4[1].add2);

}
// Funcao de fatorial
void programaFat(RAM *ram, CPU *cpu, int fat) {

    printf("\n\t\t\tExecutando programaFat (%d!)\n", fat);
    int j = 1; 

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    for (int i = 1; i <= fat; i++) {
        programaMult(ram, cpu, j, i); 

        
        Instrucao trecho1[] = {
            {3, 1, 0, 0},      // Opcode 3: Reg 1 == RAM[0]
            {5, 1, -1, -1},   //  Opcode 5 : Ganha Reg 1 para trecho1[1].add2
            {-1, 0, 0, 0}    //   Opcode -1: halt
        };
        setPrograma(cpu, trecho1);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        
        j = trecho1[1].add2; 
    }
    
    
    Instrucao trecho2[] = {
        {3, 1, 0, 0},        // Opcode 3: Reg 1 == RAM[0]
        {5, 1, -1, -1},     //  Opcode 5 : Ganha Reg 1 para trecho2[1].add2
        {-1, 0, 0, 0}      //   Opcode -1: halt
    };
    setPrograma(cpu, trecho2);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    printf("\nO resultado do fatorial eh: %d\n", trecho2[1].add2);
}

void programaSomaMatriz(RAM *ram, CPU *cpu, int cardinalidade) {

    printf("\n\t\t\tExecutando programaSomaMatriz (%dx%d)\n", cardinalidade, cardinalidade);
    
    int tamanhoMatriz = cardinalidade * cardinalidade;
    int **matriz1 = (int**)malloc(cardinalidade * sizeof(int*));
    int **matriz2 = (int**)malloc(cardinalidade * sizeof(int*));
    
    if (!matriz1 || !matriz2) {
        printf("\nErro na alocacao de matrizesn\n");
        exit(EXIT_FAILURE);
    }
    
    
    for (int i = 0; i < cardinalidade; i++) {
        matriz1[i] = (int*)malloc(cardinalidade * sizeof(int));
        matriz2[i] = (int*)malloc(cardinalidade * sizeof(int));
        for (int j = 0; j < cardinalidade; j++) {
            matriz1[i][j] = rand() % 100;
            matriz2[i][j] = rand() % 100;
        }
    }

    
    int tamanhoTotal = 3 * tamanhoMatriz;
    criarRAM_vazia(ram, tamanhoTotal);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);
    
    int endRam = 0;
    
    
    for (int i = 0; i < cardinalidade; i++) {
        for (int j = 0; j < cardinalidade; j++) {
            Instrucao trecho1[] = {
                {4, 1, matriz1[i][j], 0},  // Opcode 4 : Reg 1 == matriz2[i][j]
                {2, 1, endRam, 0},        //  Opcode 2 : RAM[1] == endRam
                {-1, 0, 0, 0} 		 //   Opcode -1: halt
            };
            setPrograma(cpu, trecho1);
            iniciarCPU(cpu, ram, &l1, &l2, &l3);
            endRam++;
        }
    }
    
    
    for (int i = 0; i < cardinalidade; i++) {
        for (int j = 0; j < cardinalidade; j++) {
            Instrucao trecho2[] = {
                {4, 1, matriz2[i][j], 0},  // Opcode 4 : Reg 1 == matriz2[i][j]
                {2, 1, endRam, 0},        //  Opcode 2 : RAM[1] == endRam
                {-1, 0, 0, 0}  		 //   Opcode -1: halt
            };
            setPrograma(cpu, trecho2);
            iniciarCPU(cpu, ram, &l1, &l2, &l3);
            endRam++;
        }
    }
    
    
    endRam = 0;
    int delta = tamanhoMatriz;
    for (int i = 0; i < tamanhoMatriz; i++) {
        Instrucao trecho3[] = {
            {0, endRam, endRam + delta, endRam + (2 * delta)},
            {-1, 0, 0, 0}  // opcode -1: halt
        };
        setPrograma(cpu, trecho3);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        endRam++;
    }
    
   
    for (int i = 0; i < cardinalidade; i++) {
        free(matriz1[i]);
        free(matriz2[i]);
    }
    free(matriz1);
    free(matriz2);
    
    printf("Soma de matrizes concluida. Resultado a partir de RAM[%d].\n", 2 * tamanhoMatriz);
    imprimirRAM(ram);
}

void programaEuclides(RAM *ram, CPU *cpu, int a, int b) {

    printf("\n\t\t\tExecutando programaEuclides (MDC de %d e %d)\n", a, b);

    
    criarRAM_vazia(ram, 2);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    Instrucao trecho1[] = {
        {4, 1, a, 0},         // Opcode 4 : Reg 1 == a
        {2, 1, 0, 0},        //  Opcode 2 : RAM[0] == Reg 1
        {4, 2, b, 0},       //   OPcode 4 : Reg 2 == b
        {2, 2, 1, 0},      //    Opcode 2 : RAM[1] == Reg 2
        {-1, 0, 0, 0}     //     Opcode -1: halt
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    int val_a = a;
    int val_b = b;

    while (val_a != val_b && val_a > 0 && val_b > 0) {
        if (val_a > val_b) {
            printf("  Subtraindo: %d = %d - %d\n", val_a - val_b, val_a, val_b);
            Instrucao trecho_sub_a[] = {
                {1, 0, 1, 0},       // Opcode 1 : Sub: RAM[0] = RAM[0] - RAM[1]
                {3, 1, 0, 0},      //  Opcode 3 : Copia: Reg 1 = RAM[0]
                {5, 1, -1, -1},   //   Opcode 5 : Obtem: Reg 1 -> trecho_sub_a[2].add2
                {-1, 0, 0, 0},   //    Opcode -1: halt
                {-1, 0, 0, 0}   //     Opcode -1: halt
            };
            setPrograma(cpu, trecho_sub_a);
            iniciarCPU(cpu, ram, &l1, &l2, &l3);
            val_a = trecho_sub_a[2].add2;

        } else if (val_b > val_a) {

            printf("  Subtraindo: %d = %d - %d\n", val_b - val_a, val_b, val_a);
            Instrucao trecho_sub_b[] = {
                {1, 1, 0, 1},      // Opcode 1 : Sub: RAM[1] = RAM[1] - RAM[0]
                {3, 2, 1, 0},     //  Opcode 3 : Copia: Reg 2 = RAM[1]
                {5, 2, -1, -1},  //   Opcode 5 : Obtem: Reg 2 -> trecho_sub_b[2].add2
                {-1, 0, 0, 0},  //    opcode -1: halt
                {-1, 0, 0, 0}  //     opcode -1: halt
            };
            setPrograma(cpu, trecho_sub_b);
            iniciarCPU(cpu, ram, &l1, &l2, &l3);
            val_b = trecho_sub_b[2].add2;
        }
    }


    int mdc_resultante = (val_a == 0) ? val_b : val_a;

    
    Instrucao trecho_salva_ram0[] = {
        {4, 1, mdc_resultante, 0}, 	// Opcode 4 : Reg 1 == mdc_resultante
        {2, 1, 0, 0}, 		       //  Opcode 2 : RAM[0] == reg1
        {-1, 0, 0, 0}   	      //   Opcode -1: halt
    };
    setPrograma(cpu, trecho_salva_ram0);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    
    Instrucao trecho_final[] = {
        {3, 1, 0, 0},	   // Opcode 3: Reg 1 == RAM[0]
        {5, 1, -1, -1},   //  Opcode 5 : Ganha Reg 1 para trecho_final[].add2
        {-1, 0, 0, 0}    //   Opcode -1: halt
    };
    setPrograma(cpu, trecho_final);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    printf("\nO resultado do MDC eh: %d\n", trecho_final[1].add2);
}

void programaMMC(RAM *ram, CPU *cpu, int a, int b) {

    printf("\n\t\t\tExecutando programaMMC (MMC de %d e %d)\n", a, b);

    int produto_ab;
    int mdc_ab;
    int mmc_final;
    
    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    Instrucao trecho_leitura_ram0[] = {
        {3, 1, 0, 0},	    // Opcode 3: Reg 1 == RAM[0]
        {5, 1, -1, -1},	   //  Opcode 5 : Ganha Reg 1 para trecho_leitura_ram0[].add2
        {-1, 0, 0, 0}     //   opcode -1: halt
    };
    

    Instrucao trecho_leitura_ram3[] = {
        {3, 1, 3, 0}, 	  // Opcode 3: Reg 1 == RAM[3]
        {5, 1, -1, -1},  //  Opcode 5 : Ganha Reg 1 para trecho_leitura_ram3[].add2
        {-1, 0, 0, 0}   //   opcode -1: halt
    };


    programaMult(ram, cpu, a, b); 
    

    setPrograma(cpu, trecho_leitura_ram0);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    produto_ab = trecho_leitura_ram0[1].add2;
    
    
    programaEuclides(ram, cpu, a, b);
    
    setPrograma(cpu, trecho_leitura_ram0);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    mdc_ab = trecho_leitura_ram0[1].add2;
    
    if (mdc_ab == 0) {
        printf("\nMDC = 0, nao eh possivel dividir para calcular MMC.\n");
        return;
    }
    programaDiv(ram, cpu, produto_ab, mdc_ab); 
    
    setPrograma(cpu, trecho_leitura_ram3);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    mmc_final = trecho_leitura_ram3[1].add2;
    printf("\nO resultado do MMC eh: %d\n", mmc_final);
}

void programaFibonat(RAM *ram, CPU *cpu, int n) {

    printf("\n\t\t\tExecutando programaFibonat (F(%d))\n", n);

    // Caso base: F(0) = 0 e F(1) = 1
    if (n <= 1) {
        printf("O resultado de F(%d) eh: %d\n", n, n);
        return;
    }


    criarRAM_vazia(ram, 3);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);
    
    Instrucao trecho1[] = {
        {4, 1, 0, 0}, //Opcode 4: load_reg: Reg 1 = 0
        {2, 1, 0, 0}, //Opcode 2: set_ram: RAM[0] = Reg 1
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    
    Instrucao trecho2[] = {
        {4, 1, 1, 0}, // load_reg: Reg 1 = 1
        {2, 1, 1, 0}, // set_ram: RAM[1] = Reg 1
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, trecho2);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    int i = 2;
    
    while (i <= n) {
        printf("  Calculando F(%d)...\n", i);
        
        Instrucao soma_fib[] = {
            {0, 0, 1, 2}, // Opcode 0: RAM[2] = RAM[0] + RAM[1]
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, soma_fib);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);


        Instrucao shift_f0[] = {
            {3, 1, 1, 0}, //Opcode 3: copy_ram_reg: Reg 1 = RAM[1] (Termo Atual)
            {2, 1, 0, 0}, //Opcode 2: set_ram: RAM[0] = Reg 1 (Termo Anterior recebe Atual)
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, shift_f0);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        

        Instrucao shift_f1[] = {
            {3, 1, 2, 0}, //Opcode 3: copy_ram_reg: Reg 1 = RAM[2] (Termo Proximo)
            {2, 1, 1, 0}, //Opcode 2: set_ram: RAM[1] = Reg 1 (Termo Atual recebe Proximo)
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, shift_f1);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        
        i++;
    }


    Instrucao trecho_final[] = {
        {3, 1, 1, 0},    // Opcode 3: copy_ram_reg: Reg 1 = RAM[1]
        {5, 1, -1, -1},  // Opcode 5: obtain_reg: trecho_final[1].add2 = Reg 1
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, trecho_final);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    int fib_final = trecho_final[1].add2;
    printf("\nO resultado de F(%d) eh: %d\n", n, fib_final);
}

void programaDet(RAM *ram, CPU *cpu, int a00, int a01, int a10, int a11) {
    printf("\n======================================\n");
    printf(" Calculando Determinante | %d %d |\n", a00, a01);
    printf("                         | %d %d |\n", a10, a11);
    printf("======================================\n");

    // --- 1. Calcula Diagonal Principal: (a00 * a11) ---
    printf("\n-> Calculando Diagonal Principal...\n");
    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);
    
    // Executa a multiplicacao (resultado vai para RAM[0])
    programaMult(ram, cpu, a00, a11);

    // Resgata o resultado da RAM[0] para a variavel C 'diagPrincipal'
    Instrucao resgateP[] = {
        {3, 1, 0, 0},      // Opcode 3: Copia RAM[0] para Reg 1
        {5, 1, -1, -1},    // Opcode 5: Traz Reg 1 para o C
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, resgateP);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    int diagPrincipal = resgateP[1].add2; // Valor resgatado


    // --- 2. Calcula Diagonal Secundaria: (a01 * a10) ---
    printf("\n-> Calculando Diagonal Secundaria...\n");
    
    // Executa a multiplicacao (resultado vai para RAM[0])
    programaMult(ram, cpu, a01, a10);

    // Resgata o resultado da RAM[0] para a variavel C 'diagSecundaria'
    Instrucao resgateS[] = {
        {3, 1, 0, 0},      // Opcode 3: Copia RAM[0] para Reg 1
        {5, 1, -1, -1},    // Opcode 5: Traz Reg 1 para o C
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, resgateS);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);
    
    int diagSecundaria = resgateS[1].add2; // Valor resgatado


    // --- 3. Realiza a Subtração na CPU: Principal - Secundaria ---
    printf("\n-> Calculando Subtracao Final (%d - %d)...\n", diagPrincipal, diagSecundaria);
    
    // Prepara a RAM para a operacao final
    criarRAM_vazia(ram, 3); // Pos 0: DiagP, Pos 1: DiagS, Pos 2: Resultado

    Instrucao programaSub[] = {
        // Carrega DiagPrincipal na RAM[0]
        {4, 1, diagPrincipal, 0}, // Reg1 = valor externo
        {2, 1, 0, 0},             // RAM[0] = Reg1

        // Carrega DiagSecundaria na RAM[1]
        {4, 1, diagSecundaria, 0}, // Reg1 = valor externo
        {2, 1, 1, 0},              // RAM[1] = Reg1

        // Executa a Subtracao: RAM[2] = RAM[0] - RAM[1]
        {1, 0, 1, 2}, 

        // Traz o resultado de RAM[2] para o registrador para podermos ler
        {3, 1, 2, 0},   // Reg1 = RAM[2]
        {5, 1, -1, -1}, // "Salva" Reg1 na variavel local C
        
        {-1, 0, 0, 0}   // HALT
    };

    setPrograma(cpu, programaSub);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    // O resultado fica salvo no segundo parametro da instrucao de Opcode 5
    int resultadoFinal = programaSub[5].add2;

    printf("\n>>> O DETERMINANTE EH: %d <<<\n", resultadoFinal);
    
    liberarRAM(ram);
}

void programaExp(RAM *ram, CPU *cpu, int base, int expoente) {
    printf("\n---  Iniciando Exponenciacao: %d elevado a %d ---\n", base, expoente);

    // Caso base: qualquer numero elevado a 0 é 1
    if (expoente == 0) {
        printf("\nO resultado da exponenciacao é: 1\n");
        return;
    }

    int acumulador = 1; // Elemento neutro da multiplicacao
    
    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    for (int i = 0; i < expoente; i++) {
        // 1. Executa a multiplicacao na CPU. 
        // O resultado ficará salvo na RAM[0] ao final desta função.
        programaMult(ram, cpu, acumulador, base);

        // 2. Precisamos resgatar o valor da RAM[0] para a variável 'acumulador' em C.
        // Faremos isso copiando RAM[0] para o Registrador e usando o Opcode 5 (Ganha Reg).
        Instrucao trechoResgate[] = {
            {3, 1, 0, 0},      // Opcode 3: Copia RAM[0] para Registrador 1
            {5, 1, -1, -1},    // Opcode 5: Obtem conteudo do Reg 1 e salva em trechoResgate[1].add2
            {-1, 0, 0, 0}      // Opcode -1: Halt
        };

        setPrograma(cpu, trechoResgate);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);

        // Atualiza o acumulador com o valor recuperado da CPU
        acumulador = trechoResgate[1].add2;
    }

    printf("\nO resultado da exponenciacao é: %d \n", acumulador);
}

void programaPrimo(RAM *ram, CPU *cpu, int n) {
    printf("\n\t\t\tExecutando programaPrimo (Testando se %d eh primo)\n", n);

    if (n <= 1) {
        printf("\n%d nao eh primo.\n", n);
        return;
    }
    if (n == 2) {
        printf("\n%d eh primo.\n", n);
        return;
    }

    //a ram precisa de 3 posicoes: 
    //ram[0]: n (o numero a ser testado)
    //ram[1]: d (o divisor atual, comeca em 2)
    //ram[2]: resultado do resto (n % d)

    criarRAM_vazia(ram, 3);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);
    
    //trecho 1: inicializa ram[0] com n e ram[1] com o primeiro divisor d=2
    Instrucao trecho1[] = {
        {4, 1, n, 0},         //opcode 4: reg 1 = n
        {2, 1, 0, 0},         //opcode 2: ram[0] = n
        {4, 2, 2, 0},         //opcode 4: reg 2 = 2
        {2, 2, 1, 0},         //opcode 2: ram[1] = 2 (d)
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    int divisor_atual = 2;
    int resto = -1; //inicializa com valor nao-zero

    //loop de teste: enquanto o divisor_atual for menor que n e o resto for diferente de zero
    while (divisor_atual < n && resto != 0) {
        
        //1. atualiza ram[1] (o divisor d) com o valor da variavel c
        Instrucao trecho_atualiza_divisor[] = {
            {4, 2, divisor_atual, 0}, //opcode 4: reg 2 = divisor_atual
            {2, 2, 1, 0},             //opcode 2: ram[1] = reg 2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_atualiza_divisor);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        
        //2. calcula o resto (modulo): ram[2] = ram[0] % ram[1]
        //opcode 6 (resto)
        Instrucao trecho_calcula_resto[] = {
            {6, 0, 1, 2},       //opcode 6: resto: ram[2] = ram[0] % ram[1]
            {3, 1, 2, 0},       //opcode 3: copia: reg 1 = ram[2] (o resto)
            {5, 1, -1, -1},     //opcode 5: obtem: reg 1 -> trecho_calcula_resto[2].add2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_calcula_resto);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        	
        resto = trecho_calcula_resto[2].add2;
        printf("\nTeste: %d %% %d = %d\n", n, divisor_atual, resto);
        
        divisor_atual++;
    }

    //trecho final: decisao
    if (resto == 0) {
        printf("\nO resultado eh: %d nao eh primo.\n", n);
    } else {
        printf("\nO resultado eh: %d eh primo.\n", n);
    }
}

void programaRaizQuadrada(RAM *ram, CPU *cpu, int n) {
    printf("\n\t\t\tExecutando programaRaizQuadrada (sqrt(%d))\n", n);

    if (n < 0) {
        printf("Erro: Nao eh possivel calcular a raiz quadrada de um numero negativo.\n");
        return;
    }
    if (n == 0 || n == 1) {
        printf("\nO resultado da raiz quadrada de %d eh: %d\n", n, n);
        return;
    }

    //a ram deve ser grande o suficiente para as operacoes auxiliares
    //posicoes: 
    //0: n (o numero) 
    //1: x_k (aproximacao atual) 
    //2: resultado de divisao (n/x_k) 
    //3: x_k+1 (proxima aproximacao)
    criarRAM_vazia(ram, 4);
    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    //inicializa ram[0] com n e ram[1] com a primeira aproximacao (x_0 = n/2 ou n/4)
    int aproximacao_inicial = n / 2;
    if (aproximacao_inicial == 0) aproximacao_inicial = 1; //garante que a divisao por zero nao ocorra
    
    Instrucao trecho1[] = {
        {4, 1, n, 0},                  //reg 1 = n
        {2, 1, 0, 0},                 // ram[0] = n
        {4, 2, aproximacao_inicial, 0}, //reg 2 = aproximacao_inicial
        {2, 2, 1, 0},                //  ram[1] = aproximacao_inicial (x_k)
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    //as aproximacoes serao feitas usando variaveis c, e a cpu apenas para calculos
    int x_k = aproximacao_inicial;
    int x_k_anterior = 0;
    int i = 0;
    
    //loop de iteracao do metodo de newton
    while (abs(x_k * x_k - n) > 1 && i < 100) { //criterio de parada: x_k^2 proximo de n
        i++;
        x_k_anterior = x_k;
        
        //1. calcula divisao (n / x_k)
        //o resultado da divisao (ram[3]) sera n / x_k (arredondado para baixo)
        programaDiv(ram, cpu, n, x_k);
        
        //trecho para ler o resultado da divisao (ram[3])
        Instrucao trecho_leitura_div[] = {
            {3, 1, 3, 0},       //reg 1 = ram[3] (n / x_k)
            {5, 1, -1, -1},    // obtem reg 1 para trecho_leitura_div[1].add2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_leitura_div);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        int n_sobre_xk = trecho_leitura_div[1].add2;
        
        //2. soma x_k + (n / x_k)
        int soma = x_k + n_sobre_xk;
        
        //3. divide a soma por 2 (para obter x_{k+1})
        //o resultado da divisao (ram[3]) sera (x_k + n/x_k) / 2
        programaDiv(ram, cpu, soma, 2);
        
        //trecho para ler o resultado final (ram[3])
        Instrucao trecho_leitura_final[] = {
            {3, 1, 3, 0},       //reg 1 = ram[3] (resultado)
            {5, 1, -1, -1},    // obtem reg 1 para trecho_leitura_final[1].add2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_leitura_final);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        
        x_k = trecho_leitura_final[1].add2;

        //se a aproximacao nao muda mais, paramos
        if (x_k == x_k_anterior) {
             break;
        }
    }
    
    printf("\nO resultado da raiz quadrada de %d eh (aproximado): %d\n", n, x_k);
}

void programaRaizCubica(RAM *ram, CPU *cpu, int n) {
    printf("\n\t\t\tExecutando programaRaizCubica (cbrt(%d))\n", n);

    if (n == 0 || n == 1 || n == -1) {
        printf("\nO resultado da raiz cubica de %d eh: %d\n", n, n);
        return;
    }

    //a ram precisa ser reiniciada ou limpa, mas vamos usar a mesma ram para os subprogramas
    criarRAM_vazia(ram, 4);

    Cache l1, l2, l3;
    inicializarCache(&l1, 16, 1);
    inicializarCache(&l2, 32, 2);
    inicializarCache(&l3, 64, 3);

    //inicializa ram[0] com n e ram[1] com a primeira aproximacao (x_0 = n/3 ou n/4)
    int sinal = (n < 0) ? -1 : 1;
    int abs_n = abs(n);
    int aproximacao_inicial = abs_n / 3;
    if (aproximacao_inicial == 0) aproximacao_inicial = 1;
    
    Instrucao trecho1[] = {
        {4, 1, abs_n, 0},             //reg 1 = |n|
        {2, 1, 0, 0},                // ram[0] = |n|
        {4, 2, aproximacao_inicial, 0},//reg 2 = aproximacao_inicial
        {2, 2, 1, 0},               //  ram[1] = aproximacao_inicial (x_k)
        {-1, 0, 0, 0}
    };
    setPrograma(cpu, trecho1);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    int x_k = aproximacao_inicial;
    int x_k_anterior = 0;
    int i = 0;
    
    //loop de iteracao do metodo de newton
    while (abs(x_k * x_k * x_k - abs_n) > 1 && i < 100) { //criterio: x_k^3 proximo de |n|
        i++;
        x_k_anterior = x_k;
        
        //1. calcula multiplicacao (x_k * x_k) = x_k^2
        programaMult(ram, cpu, x_k, x_k); 
        //resultado em ram[0]
        
        //trecho para ler o resultado da multiplicacao (ram[0])
        Instrucao trecho_leitura_mult[] = {
            {3, 1, 0, 0},       //reg 1 = ram[0] (x_k^2)
            {5, 1, -1, -1},    // obtem reg 1 para trecho_leitura_mult[1].add2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_leitura_mult);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        int xk_quadrado = trecho_leitura_mult[1].add2;
        
        if (xk_quadrado == 0) break; 
        
        //2. calcula divisao (n / x_k^2)
        //o resultado da divisao (ram[3]) sera |n| / x_k^2
        programaDiv(ram, cpu, abs_n, xk_quadrado);
        
        //trecho para ler o resultado da divisao (ram[3])
        Instrucao trecho_leitura_div[] = {
            {3, 1, 3, 0},       //reg 1 = ram[3] (|n| / x_k^2)
            {5, 1, -1, -1},    // obtem reg 1 para trecho_leitura_div[1].add2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_leitura_div);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        int n_sobre_xk2 = trecho_leitura_div[1].add2;
        
        //3. calcula 2 * x_k + (n / x_k^2)
        int soma = 2 * x_k + n_sobre_xk2;
        
        //4. divide a soma por 3 (para obter x_{k+1})
        //o resultado da divisao (ram[3]) sera (2*x_k + n/x_k^2) / 3
        programaDiv(ram, cpu, soma, 3);
        
        //trecho para ler o resultado final (ram[3])
        Instrucao trecho_leitura_final[] = {
            {3, 1, 3, 0},       //reg 1 = ram[3] (resultado)
            {5, 1, -1, -1},    // obtem reg 1 para trecho_leitura_final[1].add2
            {-1, 0, 0, 0}
        };
        setPrograma(cpu, trecho_leitura_final);
        iniciarCPU(cpu, ram, &l1, &l2, &l3);
        
        x_k = trecho_leitura_final[1].add2;

        //se a aproximacao nao muda mais, paramos
        if (x_k == x_k_anterior) {
             break;
        }
    }
    
    int resultado_final = x_k * sinal;
    printf("\nO resultado da raiz cubica de %d eh (aproximado): %d\n", n * sinal, resultado_final);
}

void programaDoArquivo(RAM *ram, CPU *cpu, char *nomeArquivo, int tamL1, int tamL2, int tamL3) {
    printf("Configuracao: L1=%d, L2=%d, L3=%d | Arquivo: %s\n", tamL1, tamL2, tamL3, nomeArquivo);

    FILE *arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: Nao foi possivel abrir o arquivo %s. Rode o gerador primeiro!\n", nomeArquivo);
        return;
    }

    // Conta quantas linhas tem para alocar
    int linhas = 0;
    char ch;
    while(!feof(arquivo)) {
        ch = fgetc(arquivo);
        if(ch == '\n') linhas++;
    }
    rewind(arquivo); // Volta pro inicio

    Instrucao *programa = alocarPrograma(linhas + 1);
    
    int op, end1, word1, end2, word2, end3, word3;
    int i = 0;

    // Formato do arquivo: op:end1:word1:end2:word2:end3:word3
    while (fscanf(arquivo, "%d:%d:%d:%d:%d:%d:%d", 
                  &op, &end1, &word1, &end2, &word2, &end3, &word3) != EOF) {
        
        programa[i].opcode = op;
        programa[i].add1 = (end1 * 4) + word1;
        programa[i].add2 = (end2 * 4) + word2;
        programa[i].add3 = (end3 * 4) + word3;
        
        i++;
    }
    programa[i].opcode = -1; // Garante o Halt no final
    fclose(arquivo);

    Cache l1, l2, l3;
    int tamanhoRAM = 4000; // 1000 blocos * 4 palavras
    
    criarRAM_vazia(ram, tamanhoRAM);
    inicializarCache(&l1, tamL1, 1);
    inicializarCache(&l2, tamL2, 2);
    inicializarCache(&l3, tamL3, 3);

    setPrograma(cpu, programa);
    iniciarCPU(cpu, ram, &l1, &l2, &l3);

    imprimirEstatisticas(&l1, &l2, &l3);

    free(l1.linhas); free(l2.linhas); free(l3.linhas);
    free(programa);
}

int main(int argc, char *argv[]) {
    RAM ram = {NULL, 0};
    CPU cpu;
    srand(time(NULL)); 

    // --- MODO AUTOMÁTICO (Para o script rodar_testes.sh) ---
    if (argc == 5) {
        int l1 = atoi(argv[1]);
        int l2 = atoi(argv[2]);
        int l3 = atoi(argv[3]);
        char *arquivo = argv[4];

        programaDoArquivo(&ram, &cpu, arquivo, l1, l2, l3);
        
        liberarRAM(&ram);
        return 0; 
    }

    // --- MODO INTERATIVO ---
    int aux = 1;
    int opcao;
    int retornoScanf;    

    while (aux == 1)
    {
        imprimeMenu();
        retornoScanf = scanf("%d", &opcao);
        
        if (retornoScanf == 0) {
            limpar_tela();
            printf("ERRO: Opcao invalida, digite novamente!\n");
            limpar_buffer();
            printf("\nPressione ENTER para voltar ao menu...");
            limpar_buffer();
            continue;
        }
        
        limpar_buffer();
        limpar_tela();

        switch (opcao)
        {
        case 1:
            programaAleatorio(&ram, &cpu, 20);
            break;

        case 2:
            programaMult(&ram, &cpu, 15, 150);
            break;

        case 3:
            programaDiv(&ram, &cpu, 175, 4);
            break;

        case 4:
            programaFat(&ram, &cpu, 10);
            break;

        case 5:
            programaEuclides(&ram, &cpu, 372, 162);
            break;

        case 6:
            programaMMC(&ram, &cpu, 8, 12);
            break;

        case 7:
            programaDet(&ram, &cpu, 2, 4, 8, 16);
            break;

        case 8:
            programaExp(&ram, &cpu, 2, 5);
            break;

        case 9:
            programaFibonat(&ram, &cpu, 10);
            break;

        case 10:
            programaPrimo(&ram, &cpu, 369);
            break;

        case 11:
            programaRaizQuadrada(&ram, &cpu, 121);
            break;

        case 12:
            programaRaizCubica(&ram, &cpu, 27);
            break;

        case 13:
            programaDoArquivo(&ram, &cpu, "instructions2.txt", 64, 128, 256);
            break;

        case 0:
            aux = 0;
            break;
        
        default:
            printf("ERRO: Opção invalida, digite novamente!\n");
            break;
        }

        if (aux != 0) {
            printf("\nPressione ENTER para voltar ao menu...");
            limpar_buffer();
        }
    }

    liberarRAM(&ram);
    return 0;
}
