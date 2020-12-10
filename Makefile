CC=gcc
CFLAGS=-Wall -pedantic -O3 -ffast-math -ffloat-store -fno-unsafe-math-optimizations -Isrc
LIBS=-lm -lGLEW -lGL -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer

EXE=bin/NewSDLOpenGLBase
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, obj/%.o, $(SRC))

DIRS=bin obj
$(info $(shell mkdir -p $(DIRS)))


all: bin/NewSDLOpenGLBase

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

$(OBJ): obj/%.o : src/%.o
	$(CC) $(CFLAGS) -c $< $(LIBS) -o $@


.PHONY: clean
clean:
	rm -rf obj $(EXE)