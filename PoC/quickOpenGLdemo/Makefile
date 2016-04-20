quickOpenGLdemo: gl3w.o
	g++ main.cpp gl3w.o -o quickOpenGLdemo -lglfw -lGL -ldl -lm
	rm -rf *.o
	rm -rf gl3w/

gl3w.o: gl3w.c
	gcc -o gl3w.o -c gl3w.c -lGL

getGl3w:
	rm -rf gl3w/
	./gl3wInstaller.sh
clean:
	rm -rfv *.o
	rm -rfv gl3w/
	rm -rfv gl3w.c
	rm -rfv quickOpenGLdemo
