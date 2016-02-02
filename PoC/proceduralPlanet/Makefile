universe: main.cpp gl3w.o heightMap.o
	g++ -o proceduralPlanet main.cpp gl3w.o heightMap.o -lglfw -lGL -ldl -lm -Wno-write-strings

gl3w.o: gl3w.c
	gcc -o gl3w.o -c gl3w.c -lGL

heightMap.o: heightMap.cpp
	g++ -c heightMap.cpp -o  heightMap.o -lm

clean:
	rm gl3w.o
	rm heightMap.o
	rm proceduralPlanet

