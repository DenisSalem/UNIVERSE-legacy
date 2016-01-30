#include <iostream>
#include <cstdlib>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "main.hpp"
#include <cmath>
#include <cstring>
#include <unistd.h>
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

Planet::Planet(int width, int height) {
	this->width = width;
	this->height = height;
	model= glm::mat4(1.0);
	view = glm::mat4(1.0);
	projection = glm::mat4(1.0);

	vertex_size = 3;

	initVertex(4);

	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	vertexShader =	"#version 130\n"
			"in vec3 vertex;\n"
			"in vec3 vertexColor;\n"
			"uniform mat4 model;"
			"uniform mat4 view;"
			"uniform mat4 projection;"
			"void main() {\n"
			"	gl_Position = projection * view * model * vec4(vertex.xyz ,1.0);\n"
			"	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;\n"
			"	gl_FrontColor = vec4(vertexColor.x+0.5, vertexColor.y+0.5, vertexColor.z+0.5, 1.0);\n"
			"}";


	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	fragmentShader =	"#version 130\n"
				"out vec4 color;\n" 
				"void main() {\n"
				"	color = gl_Color;\n"
				"}\n";

	initVBO();
	initVAO();
	this->loadVertex();
	this->loadShader();
}

Planet::~Planet() {
	glDeleteBuffers(1, &this->VBO);
}

void Planet::initVBO() {
	if(glIsBuffer(this->VBO) == GL_TRUE) {
        	glDeleteBuffers(1, &this->VBO);
	}

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Planet::initVAO() {
	if(glIsVertexArray(this->VAO) == GL_TRUE) {
        	glDeleteVertexArrays(1, &this->VAO);
	}
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(0);
}

void Planet::loadVertex() {
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->vertex_number * this->vertex_size * 2, 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * this->vertex_number * this->vertex_size, this->vertex_array);
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * this->vertex_number * this->vertex_size, sizeof(float) * this->vertex_number * this->vertex_size, this->vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(float) * this->vertex_number * this->vertex_size));
			glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Planet::compileShader(GLuint shaderID, char * sourcePointer) {
	glShaderSource(shaderID, 1, &sourcePointer , NULL);
	glCompileShader(shaderID);

	GLint Result = GL_FALSE;
	int InfoLogLength = 1024;
	char shaderErrorMessage[1024] = {0};

	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	
	glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage);
	if (strlen(shaderErrorMessage) != 0) 
		std::cout <<  shaderErrorMessage << "\n" ;
}

void Planet::loadShader() {
	this->compileShader(this->vertexShaderID, this->vertexShader);
	this->compileShader(this->fragmentShaderID, this->fragmentShader);

	this->programID = glCreateProgram();

	glAttachShader(this->programID, this->vertexShaderID);
	glAttachShader(this->programID, this->fragmentShaderID);

	glLinkProgram(this->programID);

	GLint Result = GL_FALSE;
	int InfoLogLength = 1024;
	char ProgramErrorMessage[1024] = {0};

	glGetProgramiv(this->programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(this->programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	glGetProgramInfoLog(this->programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	if (strlen(ProgramErrorMessage) != 0) 
		std::cout << ProgramErrorMessage << "\n";
}

void Planet::render() {
	glUseProgram(this->programID);

	this->model = glm::rotate(this->model, (glm::mediump_float) 0.09, glm::vec3(0.0,0.0,1.0));
	this->model = glm::rotate(this->model, (glm::mediump_float) 0.03, glm::vec3(0.0,1.0,0.0));
	this->model = glm::rotate(this->model, (glm::mediump_float) 0.01, glm::vec3(1.0,0.0,0.0));
	this->view = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.0f, -2.00f));
	this->projection = glm::perspective(45.0, (double) this->width/this->height, 0.1, 10000.0);

	glBindVertexArray(this->VAO);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUniformMatrix4fv(glGetUniformLocation(this->programID, "model"), 1, GL_FALSE, glm::value_ptr(this->model));
		glUniformMatrix4fv(glGetUniformLocation(this->programID, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
		glUniformMatrix4fv(glGetUniformLocation(this->programID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));

		glDrawArrays(GL_TRIANGLES,0,this->vertex_number * this->vertex_size);
	glBindVertexArray(0);
	glUseProgram(0);

}

void Planet::initVertex(int iteration) {

	this->vertex_number = 6 * (int) pow(4, iteration-1) * 2 * 3;
	std::cout << "Initiate Cube with " << this->vertex_number << " vertex.\n";

	this->vertex_array = new float[this->vertex_number * this->vertex_size];
	for (int i=0;i<this->vertex_number * this->vertex_size; i++) {
		this->vertex_array[i] = 0.0f;
	}

	int size = pow(2,iteration-1);
	float step = 1.0 / size;	

	int offset= 18;

	for(int x = 0; x < size; x++) {
		for( int y = 0; y < size; y++) {
			
			//First Face
			this->vertex_array[ 18 * (x * size + y)     ]	= -0.5 + x * step;
			this->vertex_array[ 18 * (x * size + y) + 1 ] 	= -0.5 + y * step;
			this->vertex_array[ 18 * (x * size + y) + 2 ] 	= 0.5;

			this->vertex_array[ 18 * (x * size + y) + 3 ]	= -0.5 + x * step;
			this->vertex_array[ 18 * (x * size + y) + 4 ] 	= -0.5 + y * step + step;
			this->vertex_array[ 18 * (x * size + y) + 5 ] 	= 0.5;

			this->vertex_array[ 18 * (x * size + y) + 6 ]	= -0.5 + x * step + step;
			this->vertex_array[ 18 * (x * size + y) + 7 ] 	= -0.5 + y * step + step;
			this->vertex_array[ 18 * (x * size + y) + 8 ] 	= 0.5;

			this->vertex_array[ 18 * (x * size + y) + 9 ]	= -0.5 + x * step;
			this->vertex_array[ 18 * (x * size + y) + 10] 	= -0.5 + y * step;
			this->vertex_array[ 18 * (x * size + y) + 11] 	= 0.5;

			this->vertex_array[ 18 * (x * size + y) + 12 ]	= -0.5 + x * step + step;
			this->vertex_array[ 18 * (x * size + y) + 13 ] 	= -0.5 + y * step;
			this->vertex_array[ 18 * (x * size + y) + 14 ] 	= 0.5;

			this->vertex_array[ 18 * (x * size + y) + 15 ]	= -0.5 + x * step + step;
			this->vertex_array[ 18 * (x * size + y) + 16 ] 	= -0.5 + y * step + step;
			this->vertex_array[ 18 * (x * size + y) + 17 ] 	= 0.5;
		}
	}
	offset = 18 * size * size;
	for(int x = 0; x < size; x++) {
		for( int y = 0; y < size; y++) {
			// Second Face
			this->vertex_array[ offset + 18 * (x * size + y) ]	= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 1 ] = -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 2 ] = -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 3 ]	= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 4 ] = -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 5 ] = -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 6 ]	= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 7 ] = -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 8 ] = -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 9]	= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 10] 	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 11] 	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 12 ]	= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 13 ] = -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 14 ] = -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 15 ]	= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 16 ] = -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 17 ] = -0.5 + x * step;
		}
	}
	
	offset = 18 * size * size * 2;
	for(int x = 0; x < size; x++) {
		for( int y = 0; y < size; y++) {
			// Third Face
			this->vertex_array[ offset + 18 * (x * size + y) ]	= -0.5 + x * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 1 ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 2 ]	= -0.5;

			this->vertex_array[ offset + 18 * (x * size + y) + 3 ]	= -0.5 + x * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 4 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 5 ]	= -0.5;

			this->vertex_array[ offset + 18 * (x * size + y) + 6 ]	= -0.5 + x * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 7 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 8 ]	= -0.5;

			this->vertex_array[ offset + 18 * (x * size + y) + 9 ]	= -0.5 + x * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 10] 	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 11] 	= -0.5;

			this->vertex_array[ offset + 18 * (x * size + y) + 12 ]	= -0.5 + x * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 13 ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 14 ]	= -0.5;

			this->vertex_array[ offset + 18 * (x * size + y) + 15 ]	= -0.5 + x * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 16 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18* (x * size + y) + 17 ] 	= -0.5;
		}
	}
	offset = 18 * size * size * 3;
	for(int x = 0; x < size; x++) {
		for( int y = 0; y < size; y++) {
			//Fourth Face
			this->vertex_array[ offset + 18 * (x * size + y) ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 1 ] 	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 2 ] 	= -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 3 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 4 ] 	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 5 ] 	= -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 6 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 7 ] 	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 8 ] 	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 9 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 10] 	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 11] 	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 12 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 13 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 14 ]	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 15 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 16 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 17 ]	= -0.5 + x * step;
		}
	}
	offset = 18 * size * size * 4;
	for(int x = 0; x < size; x++) {
		for( int y = 0; y < size; y++) {
			//Fifth Face
			this->vertex_array[ offset + 18 * (x * size + y) ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 1 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 2 ]	= -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 3 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 4 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 5 ]	= -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 6 ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 7 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 8 ]	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 9 ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 10] 	= 0.5;
			this->vertex_array[ offset + 18* (x * size + y) + 11] 	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 12 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 13 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 14 ]	= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 15 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 16 ]	= 0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 17 ]	= -0.5 + x * step;
		}
	}
	offset = 18 * size * size * 5;
	for(int x = 0; x < size; x++) {
		for( int y = 0; y < size; y++) {
			//Sixth Face
			this->vertex_array[ offset + 18 * (x * size + y) ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 1 ] = -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 2 ] = -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 3 ]	= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 4 ] = -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 5 ] = -0.5 + x * step;

			this->vertex_array[ offset + 18 * (x * size + y) + 6 ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 7 ] = -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 8 ] = -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 9 ]	= -0.5 + y * step;
			this->vertex_array[ offset + 18 * (x * size + y) + 10] = -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 11] = -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 12 ]= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 13 ]= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 14 ]= -0.5 + x * step + step;

			this->vertex_array[ offset + 18 * (x * size + y) + 15 ]= -0.5 + y * step + step;
			this->vertex_array[ offset + 18 * (x * size + y) + 16 ]= -0.5;
			this->vertex_array[ offset + 18 * (x * size + y) + 17 ]= -0.5 + x * step;
		}
	}
} 

int main(void) {
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
	
	Planet planet = Planet(WINDOW_WIDTH, WINDOW_HEIGHT);

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window)) {
		usleep( 40 * 1000 );
		planet.render();
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}
