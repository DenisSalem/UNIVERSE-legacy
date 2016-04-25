proceduralPlanet: gl3w world common
	g++ -o proceduralPlanet main.cpp *.o -ldl -lglfw -lGL
	rm -rf *.o
	rm -rf gl3w/

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

gl3w: gl3w.c
	gcc -o gl3w.o -c gl3w.c

getGl3w:
	rm -rf gl3w/
	./gl3wInstaller.sh

clean:
	rm -rfv *.o
	rm -rfv gl3w/
	rm -rfv gl3w.c
	rm -rfv proceduralPlanet
