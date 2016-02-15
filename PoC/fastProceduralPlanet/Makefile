universe: planet.o gl3w.o
	g++ main.cpp planet.o gl3w.o -o universe -lglfw -lGL -ldl -lm -Wno-write-strings
gl3w.o: gl3w.c
	gcc -o gl3w.o -c gl3w.c -lGL
planet.o: planet.cpp
	g++ -c planet.cpp -o planet.o
clean:
	rm planet.o
	rm gl3w.o
	rm universe
