CC = gcc
CFLAGS = -Wall
LIBS = -lpthread -lncurses

SRC_DIR = src
INC_DIR = inc
OBJ_DIR = obj
BIN_DIR = bin

_OBJ = client.o
OBJ = $(patsubst %,$(OBJ_DIR)/%,$(_OBJ))

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c -o $@ $<

$(BIN_DIR)/client: $(OBJ) | $(BIN_DIR)
	$(CC) -o $@ $^ $(LIBS)

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

.PHONY: all
all: $(BIN_DIR)/client

.PHONY: clean
clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/client
