quickOpenGLdemo: gl3w.o
	g++ main.cpp gl3w.o -o quickOpenGLdemo -lglfw -lGL -ldl -lm
gl3w.o: gl3w.c
	gcc -o gl3w.o -c gl3w.c -lGL
clean:
	rm gl3w.o
	rm quickOpenGLdemo
