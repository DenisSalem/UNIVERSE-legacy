universe: main.cpp gl3w.o
	g++ -o proceduralPlanet main.cpp gl3w.o -lglfw -lGL -ldl -lm -Wno-write-strings

gl3w.o: gl3w.c
	gcc -o gl3w.o -c gl3w.c -lGL
clean:
	rm gl3w.o
	rm universe

