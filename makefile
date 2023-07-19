cc = g++
CFLAGS = -Wall -Wextra

all:
	$(cc) $(CFLAGS) -o main main.c loadShader.cpp -lGL -lglfw -lGLEW

run: all
	./main
