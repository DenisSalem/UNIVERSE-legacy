#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "main.hpp"
#include <cmath>
#include <cstring>
#include <unistd.h>

int main(int argc, char ** argv) {
	GLFWwindow* window;

	if (!glfwInit())
		exit(-1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Procedural Planet", NULL, NULL);

	if (!window){
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window); 

	if(gl3wInit() != 0) {
		std::cout << "gl3w fail\n";
		return 0;
	};
	std::cout << glGetString(GL_VERSION) << "\n";
	
	glEnable(GL_DEPTH_TEST);

	Planet planet = Planet(atoi(argv[1]));

	while (!glfwWindowShouldClose(window)) {
		usleep( 40 * 1000 );
		planet.RENDER(WINDOW_WIDTH, WINDOW_HEIGHT);
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}
