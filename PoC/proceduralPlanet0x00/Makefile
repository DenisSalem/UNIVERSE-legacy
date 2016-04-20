proceduralPlanet: gl3w world common
	g++ -o proceduralPlanet main.cpp *.o -ldl -lglfw -lGL
	make clean

world: planetFactory renderPlanet
	g++ -o planet.o -c world/world.cpp
planetFactory: CubeMap
	g++ -o planetFactory.o -c world/planetFactory.cpp

renderPlanet: loadAndCompileShader
	g++ -o renderPlanet.o -c world/renderPlanet.cpp

CubeMap:
	g++ -o cubeMap.o -c world/cubeMap.cpp	

loadAndCompileShader:
	g++ -o loadShader.o -c shaders/loadShader.cpp
	g++ -o compileShader.o -c shaders/compileShader.cpp

common:
	g++ -o common.o -c common.cpp
gl3w:
	gcc -o gl3w.o -c gl3w.c

clean:
	rm *.o
