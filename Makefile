INCLUDES= -I ./include
LIBS= -L ./lib
FLAGS= -g
OBJECTS= ./build/chip8memory.o ./build/chip8stack.o ./build/chip8keyboard.o ./build/shadowself.o ./build/chip8screen.o

all: ${OBJECTS}
	gcc ${FLAGS} ${INCLUDES} ./src/main.c ${OBJECTS} -lSDL2main -lSDL2 -o ./bin/main

./build/chip8memory.o:./src/chip8memory.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8memory.c -c -o ./build/chip8memory.o

./build/chip8stack.o:./src/chip8stack.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8stack.c -c -o ./build/chip8stack.o

./build/chip8keyboard.o:./src/chip8keyboard.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8keyboard.c -c -o ./build/chip8keyboard.o

./build/shadowself.o:./src/shadowself.c
	gcc ${FLAGS} ${INCLUDES} ./src/shadowself.c -c -o ./build/shadowself.o

./build/chip8screen.o:./src/chip8screen.c
	gcc ${FLAGS} ${INCLUDES} ./src/chip8screen.c -c -o ./build/chip8screen.o

clean:
	rm ./build/*
