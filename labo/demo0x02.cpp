/*

	gcc -c gl3w.c -o gl3w.o
	$ g++ demo0x02.cpp gl3w.o -lGL -ldl -lglfw -Wno-write-strings
*/

#include <iostream>
#include <cstdlib>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

void initVAO(GLuint * VertexArrayID) {
	glGenVertexArrays(1, VertexArrayID);
	glBindVertexArray(*VertexArrayID);
}

void initVBO(GLuint * vertexbuffer, GLfloat * g_vertex_buffer_data) {
	glGenBuffers(1, vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, *vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9, g_vertex_buffer_data, GL_STATIC_DRAW);
}

void compileShader(GLuint shaderID, char * sourcePointer) {
	glShaderSource(shaderID, 1, &sourcePointer , NULL);
	glCompileShader(shaderID);

	GLint Result = GL_FALSE;
	int InfoLogLength = 1024;
	char shaderErrorMessage[1024] = {0};

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage);
	std::cout <<  shaderErrorMessage << "\n" ;
}

void render(GLuint vertexbuffer) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   0            // array buffer offset
	);
 
	glDrawArrays(GL_TRIANGLES, 0, 3); 
 
	glDisableVertexAttribArray(0);
}

static void error_callback(int error, const char* description)
{
	std::cout << description ;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(-1);

	// use Core OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	window = glfwCreateWindow(640, 480, "untitled", NULL, NULL);

	if (!window){
		glfwTerminate();
		exit(-1);
	}

	glfwMakeContextCurrent(window); 

	glfwSetKeyCallback(window, key_callback);
	if(gl3wInit() != 0) {
		std::cout << "gl3w fail\n";
		return 0;
	};
	std::cout << glGetString(GL_VERSION) << "\n";
	
	GLuint vertexbuffer;
	GLuint VertexArrayID;
	GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	GLchar * vertexShaderSource = 	"#version 330 core\n"
					"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
					"void main() {\n"
					"	gl_Position.xyz = vertexPosition_modelspace;\n"
					"	gl_Position.w = 1.0;\n"
					"}";

	GLchar * fragmentShaderSource =	"#version 330\n"
					"out vec3 color;\n"
					"void main() {\n"
					"	color = vec3(1,0,0);\n"
					"}";

	initVAO(&VertexArrayID);
	initVBO(&vertexbuffer, g_vertex_buffer_data);

	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	compileShader(VertexShaderID, vertexShaderSource);
	compileShader(FragmentShaderID, fragmentShaderSource);

	GLuint ProgramID = glCreateProgram();

    	glAttachShader(ProgramID, VertexShaderID);
    	glAttachShader(ProgramID, FragmentShaderID);
    	glLinkProgram(ProgramID);

	GLint Result = GL_FALSE;
	int InfoLogLength = 1024;
	char ProgramErrorMessage[1024] = {0};

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	std::cout << ProgramErrorMessage << "\n";

	glUseProgram(ProgramID);

	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		render(vertexbuffer);

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}
