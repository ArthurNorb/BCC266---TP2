#!/bin/bash

# Compila o projeto para garantir que está atualizado
make

# Verifica se o arquivo de instruções existe
if [ ! -f "instructions2.txt" ]; then
    echo "Gerando arquivo de instruções..."
    ./gerador_instrucoes
fi

echo "==========================================================="
echo "INICIANDO BATERIA DE TESTES AUTOMATIZADOS (TP2)"
echo "==========================================================="
echo "Maquina | L1  | L2  | L3  | Resultado"
echo "-----------------------------------------------------------"

# Função para rodar e filtrar a saída
rodar_teste() {
    NOME=$1
    L1=$2
    L2=$3
    L3=$4
    
    echo "Rodando $NOME (L1=$L1, L2=$L2, L3=$L3)..."
    
    # Roda o programa e pega apenas as linhas de estatísticas
    # O comando 'grep' filtra só o que queremos ver
    ./tp2_maquina $L1 $L2 $L3 instructions2.txt | grep "Hits:"
    
    echo "-----------------------------------------------------------"
}

# Configurações da Tabela do PDF
# M1: 8, 16, 32
rodar_teste "M1" 8 16 32

# M2: 32, 64, 128
rodar_teste "M2" 32 64 128

# M3: 16, 64, 256
rodar_teste "M3" 16 64 256

# M4: 8, 32, 128
rodar_teste "M4" 8 32 128

# M5: 16, 32, 64
rodar_teste "M5" 16 32 64

echo "FIM DOS TESTES."