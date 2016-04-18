proceduralSphere: gl3w sphere
	g++ -o proceduralSphere main.cpp *.o -ldl -lglfw -lGL -O02
	make clean

sphere: vertexFactory renderSphere
	g++ -o sphere.o -c sphere/sphere.cpp
vertexFactory:
	g++ -o vertexFactory.o -c sphere/vertexFactory.cpp

renderSphere: loadAndCompileShader
	g++ -o renderSphere.o -c sphere/renderSphere.cpp
	
loadAndCompileShader:
	g++ -o loadShader.o -c shaders/loadShader.cpp
	g++ -o compileShader.o -c shaders/compileShader.cpp

gl3w: gl3w.c
	gcc -o gl3w.o -c gl3w.c

clean:
	rm *.o
