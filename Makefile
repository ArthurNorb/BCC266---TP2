# Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Nome do executável final
TARGET = tp2_maquina
# Nome do executável do gerador de instruções
GEN_TARGET = gerador_instrucoes

# Arquivos fontes (.c) do projeto principal
SRCS = Programas.c CPU.c RAM.c MMU.c
# Arquivos objetos (.o) gerados automaticamente
OBJS = $(SRCS:.c=.o)

# --- REGRAS ---

# 'make all' ou apenas 'make' compila tudo
all: $(TARGET) $(GEN_TARGET)

# Regra para criar o executável principal
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Regra para criar o gerador de instruções separadamente
$(GEN_TARGET): instructionsGenerator.c
	$(CC) $(CFLAGS) instructionsGenerator.c -o $(GEN_TARGET)

# Regra genérica para transformar qualquer .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 'make clean' apaga os arquivos compilados para limpar a pasta
clean:
	rm -f $(OBJS) $(TARGET) $(GEN_TARGET) *.o

# 'make run' compila e roda o programa principal
run: $(TARGET)
	./$(TARGET)