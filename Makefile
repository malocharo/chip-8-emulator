OBJS = main.c cpu.c stack.c display.c

CC = gcc

COMPILER_FLAG = -Wall

LINKER_FLAG = -lSDL2

OBJ_NAME = chip8

all: $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAG) $(LINKER_FLAG) -o $(OBJ_NAME)

clean:
	rm *.o *.gch