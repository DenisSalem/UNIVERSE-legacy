#include <iostream>
#include <unistd.h>

#include "common.hpp"
#include "sphere/sphere.hpp"

int main(int argc, char ** argv) {
  GLFWwindow* window;

  if (!glfwInit()) {
    std::cout << "glfw failed, aborting...\n";
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Procedural Sphere", NULL, NULL);

  if (!window){
    std::cout << "No window available, aborting...";
    glfwTerminate();
    exit(-1);
  }

  glfwMakeContextCurrent(window); 

  if(gl3wInit() != 0) {
    std::cout << "gl3w failed, aborting...\n";
    exit(-1);
  };

  std::cout << glGetString(GL_VERSION) << "\n";
	
  glEnable(GL_DEPTH_TEST);

  Sphere sphere = Sphere(atoi(argv[1]));

  while (!glfwWindowShouldClose(window)) {
    usleep( 40 * 1000 );
    sphere.renderSphere->Render(WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);

  glfwTerminate();

  exit(EXIT_SUCCESS);
}
