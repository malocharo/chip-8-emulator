CC=gcc
CFLAGS = -Wall

chip8: main.o stack.o cpu.o
	$(CC) -o chip8 main.o stack.o cpu.o

main.o: main.c cpu.h
	$(CC) $(CFLAGS) -c main.c 

stack.o: stack.c 
	$(CC) $(CFLAGS) -c stack.c stack.h

cpu.o: cpu.c 
	$(CC) $(CFLAGS) -c cpu.c cpu.h

clean:
	rm *.o *.gch