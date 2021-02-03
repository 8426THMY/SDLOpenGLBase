CC=gcc
CFLAGS=-Wall -pedantic -O3 -ffast-math -ffloat-store -fno-unsafe-math-optimizations -Isrc
ifeq ($(OS), Windows_NT)
	LIBS=-lglew32s -lmingw32 -lopengl32 -lwinmm
	EXE=bin/NewSDLOpenGLBase.exe
else
	LIBS=-lm -lGLEW -lGL
	EXE=bin/NewSDLOpenGLBase
endif
LIBS+=-lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lrt

SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c, obj/%.o, $(SRC))

DIRS=bin obj
$(info $(shell mkdir -p $(DIRS)))


all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ $(LIBS)

$(OBJ): obj/%.o : src/%.c
	$(CC) $(CFLAGS) -c $< $(LIBS) -o $@


.PHONY: clean
clean:
	rm -rf obj $(EXE)