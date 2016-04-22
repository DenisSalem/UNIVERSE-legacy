proceduralSphere: gl3w sphere
	g++ -o proceduralSphere main.cpp *.o -ldl -lglfw -lGL -O02
	rm -rf *.o
	rm -rf gl3w/

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

getGl3w:
	rm -rf gl3w/
	./gl3wInstaller.sh

clean:
	rm -rfv *.o
	rm -rfv gl3w/
	rm -rfv gl3w.c
	rm -rfv proceduralSphere
