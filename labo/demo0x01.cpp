/* compile with * g++ -lglfw -lGL -lm demo0x01.cpp */

#include <iostream>
#include <GLFW/glfw3.h>
#include <math.h>
void render(GLFWwindow* window, float r, float g, float b) {
	glClearColor(r,g,b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glfwSwapBuffers(window);
}

int main(void) {
	GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    std::cout << glGetString(GL_VERSION) << "\n";
    float r,g,b,i = 0;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
	r = cos(i)	* 0.5 + 0.5;
	g = cos(i*2)	* 0.5 + 0.5;
	b = cos(i*4)	* 0.5 + 0.5;
	render(window, r,g,b);
	i = i + 0.01;
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
