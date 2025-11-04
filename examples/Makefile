CC       = cc
CFLAGS   = -Wall -Wextra -std=c11 -I..
BIN_DIR  = ../bin
OBJ_DIR  = obj

RAYLIB_LIBS = -lraylib -lm -lpthread -ldl -lrt -lX11
SDL3_LIBS   = -lSDL3 -lm -ldl -lpthread

EXAMPLES = raylib_example sdl3_example
OBJS     = $(OBJ_DIR)/assman.o

all: $(EXAMPLES:%=$(BIN_DIR)/%)

raylib_example: raylib_example.c ../assman.c
	$(CC) $(CFLAGS) -I.. $^ \
		-L/usr/local/lib -lraylib -lm -lpthread -ldl -lrt -lX11 \
		-o $@

$(BIN_DIR)/sdl3_example: $(OBJ_DIR)/sdl3_example.o $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $^ -o $@ $(SDL3_LIBS)

$(OBJ_DIR)/assman.o: ../assman.c ../assman.h
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean

