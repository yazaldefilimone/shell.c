# vars
CC = clang
CFLAGS = -Wall -Wextra -O2
SRC_DIR = .
BUILD_DIR = build
TARGET = $(BUILD_DIR)/core

# Arquivos fonte
SRCS = $(wildcard $(SRC_DIR)/*.c)
# Arquivos objeto
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Regra padrão
all: $(TARGET)

# Regra para criar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regra para criar arquivos objeto
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Limpeza
clean:
	rm -rf $(BUILD_DIR)

# PHONY targets
.PHONY: all clean
