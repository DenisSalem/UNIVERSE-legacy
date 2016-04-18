proceduralPlanet: gl3w world
	g++ -o proceduralPlanet main.cpp *.o -ldl -lglfw -lGL
	make clean

world: planetFactory renderPlanet
	g++ -o planet.o -c world/world.cpp
planetFactory:
	g++ -o planetFactory.o -c world/planetFactory.cpp

renderPlanet: loadAndCompileShader
	g++ -o renderPlanet.o -c world/renderPlanet.cpp
	
loadAndCompileShader:
	g++ -o loadShader.o -c shaders/loadShader.cpp
	g++ -o compileShader.o -c shaders/compileShader.cpp

gl3w: gl3w.c
	gcc -o gl3w.o -c gl3w.c

clean:
	rm *.o
