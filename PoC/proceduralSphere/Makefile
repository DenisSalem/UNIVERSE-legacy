proceduralSphere: planet.o gl3w.o
	g++ main.cpp planet.o gl3w.o -o proceduralSphere -lglfw -lGL -ldl -lm -O02
gl3w.o: gl3w.c
	gcc -o gl3w.o -c gl3w.c -lGL
planet.o: planet.cpp
	g++ -c planet.cpp -o planet.o -Wno-int-to-pointer-cast
clean:
	rm planet.o
	rm gl3w.o
	rm proceduralSphere
