CC = gcc
CFLAGS = -I/usr/include/postgresql -I/usr/local/include
LDFLAGS = -lpq -ljansson
SRC_DIR = src

SRCS = $(SRC_DIR)/crud.c $(SRC_DIR)/funciones.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)
TARGET = crud

all: $(TARGET)

$(TARGET): $(OBJS)
	@$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Regla para construir los archivos objeto
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Regla para limpiar los archivos generados
clean:
	@rm -f $(TARGET) $(OBJS)
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
