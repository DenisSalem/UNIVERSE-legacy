/*
	gcc -c gl3w.c -o gl3w.o
	$ g++ demo0x04.cpp gl3w.o -lGL -ldl -lglfw -Wno-write-strings
*/

#include <iostream>
#include <cstdlib>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void initVAO(GLuint * VertexArrayID) {
	/* generate vertex array object names */
	glGenVertexArrays(1, VertexArrayID);
	/* Bind VAO with VertexArrayID */
	glBindVertexArray(*VertexArrayID);
}

void initVBO(GLuint * vertexbuffer, GLfloat * g_vertex_buffer_data) {
	/* As Above, generate a buffer */
	glGenBuffers(1, vertexbuffer);
	/* then bind it to vertexbuffer */
	glBindBuffer(GL_ARRAY_BUFFER, *vertexbuffer);
	/* 
	void glBufferData(GLenum  target,  GLsizeiptr  size,  const GLvoid *  data,  GLenum  usage);

		glBufferData creates a new data store for the buffer object currently bound to
        	target. Any pre-existing data store is deleted. The new data store is created with the
        	specified size in bytes and usage. If data
	        is not NULL, the data store is initialized with data from this pointer. 
	*/

	// Because we do not want to display only one triangle but a cube we must set the buffer to 12 triangles
	// 2 triangles per faces and 9 float for a triangle (3 float per vertex)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*12, g_vertex_buffer_data, GL_STATIC_DRAW);
}

void compileShader(GLuint shaderID, char * sourcePointer) {
	glShaderSource(shaderID, 1, &sourcePointer , NULL);
	glCompileShader(shaderID);

	GLint Result = GL_FALSE;
	/* prepare buffer for receiving compile log */
	int InfoLogLength = 1024;
	char shaderErrorMessage[1024] = {0};

	/* return a parameter from a shader object

	GL_COMPILE_STATUS
		For implementations that support a shader compiler, 
		    params returns GL_TRUE if the last compile operation on shader was
		    successful, and GL_FALSE otherwise.
	
		result isn't actually used, but remain for debugging purpose.
	*/
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	
	/* get the actual log of last compile in shaderErrorMessage array */ 
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
 
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
	glDisableVertexAttribArray(0);
}

static void error_callback(int error, const char* description)
{
	std::cout << description ;
}

int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(-1);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	window = glfwCreateWindow(640, 480, "Hello White Triangle", NULL, NULL);

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
	
	GLuint vertexbuffer;
	GLuint VertexArrayID;
	GLfloat g_vertex_buffer_data[] = {
		-1.0f,-1.0f,-1.0f, // triangle 1 : begin
		-1.0f,-1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f, // triangle 1 : end
		1.0f, 1.0f,-1.0f, // triangle 2 : begin
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f, // triangle 2 : end
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f,-1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f,-1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f,-1.0f, 1.0f
	};
	GLchar * vertexShaderSource = 	"#version 330 core\n"
					"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
					"uniform mat4 MVP;\n"
					"void main() {\n"
					"	vec4 v = vec4(vertexPosition_modelspace,1);\n"
					"	gl_Position = MVP * v;\n"
					"}";

	GLchar * fragmentShaderSource =	"#version 330\n"
					"out vec3 color;\n"
					"void main() {\n"
					"	color = vec3(1,1,1);\n"
					"}";

	/* Init VAO and VBO */
	initVAO(&VertexArrayID);
	initVBO(&vertexbuffer, g_vertex_buffer_data);


	/* Create both fragment and vertex shader */
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	/* compile them */
	compileShader(VertexShaderID, vertexShaderSource);
	compileShader(FragmentShaderID, fragmentShaderSource);

	/* create program */
	GLuint ProgramID = glCreateProgram();

        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        glm::mat4 Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        // Or, for an ortho camera :
        //glm::mat4 Projection = glm::ortho(-10.0f,10.0f,-10.0f,10.0f,0.0f,100.0f); // In world coordinates

        // Camera matrix
        glm::mat4 View       = glm::lookAt(
                                                                glm::vec3(4,3,-3), // Camera is at (4,3,3), in World Space
                                                                glm::vec3(0,0,0), // and looks at the origin
                                                                glm::vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
                                                   );
        // Model matrix : an identity matrix (model will be at the origin)
        glm::mat4 Model      = glm::mat4(1.0f);
        // Our ModelViewProjection : multiplication of our 3 matrices
        glm::mat4 MVP        = Projection * View * Model; // Remember, matrix multiplication is the other way around

	/* Bind shaders to program */
    	glAttachShader(ProgramID, VertexShaderID);
    	glAttachShader(ProgramID, FragmentShaderID);

	/* Link it */
    	glLinkProgram(ProgramID);

	// Get a handle for our "MVP" uniform
	// must be AFTER liniking
        GLuint MatrixID = glGetUniformLocation(ProgramID, "MVP");

	GLint Result = GL_FALSE;
	int InfoLogLength = 1024;
	char ProgramErrorMessage[1024] = {0};

	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
	std::cout << ProgramErrorMessage << "\n";

	/* Installs a program object as part of current rendering state */


	while (!glfwWindowShouldClose(window)) {
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	
		glUseProgram(ProgramID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		render(vertexbuffer);
		/* WHOA COOL HOW/WHERE THE FUCK SHADERS ARE LINKED WITH VAO?? */

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}
