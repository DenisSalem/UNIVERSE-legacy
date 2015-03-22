/*
	gcc -c gl3w.c -o gl3w.o
	$ g++ demo0x05.cpp gl3w.o -lGL -ldl -lglfw -Wno-write-strings
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

void initVBO(GLuint * buffer, GLfloat * g_buffer_data) {
	/* As Above, generate a buffer */
	glGenBuffers(1, buffer);
	/* then bind it to vertexbuffer */
	glBindBuffer(GL_ARRAY_BUFFER, *buffer);
	/* 
	void glBufferData(GLenum  target,  GLsizeiptr  size,  const GLvoid *  data,  GLenum  usage);

		glBufferData creates a new data store for the buffer object currently bound to
        	target. Any pre-existing data store is deleted. The new data store is created with the
        	specified size in bytes and usage. If data
	        is not NULL, the data store is initialized with data from this pointer. 
	*/

	// Because we do not want to display only one triangle but a cube we must set the buffer to 12 triangles
	// 2 triangles per faces and 9 float for a triangle (3 float per vertex)
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9*12, g_buffer_data, GL_STATIC_DRAW);
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

void render(GLuint vertexBuffer, GLuint colorBuffer) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexAttribPointer(
	   0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   0            // array buffer offset
	);
 
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
	glVertexAttribPointer(
	   1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	   3,                  // size
	   GL_FLOAT,           // type
	   GL_FALSE,           // normalized?
	   0,                  // stride
	   0            // array buffer offset
	);
	glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles -> 6 squares
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
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
	
	GLuint vertexBuffer;
	GLuint colorBuffer;
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

	// One color for each vertex. They were generated randomly.
	GLfloat g_color_buffer_data[] = {
		0.583f,  0.771f,  0.014f,
		0.609f,  0.115f,  0.436f,
		0.327f,  0.483f,  0.844f,
		0.822f,  0.569f,  0.201f,
		0.435f,  0.602f,  0.223f,
		0.310f,  0.747f,  0.185f,
		0.597f,  0.770f,  0.761f,
		0.559f,  0.436f,  0.730f,
		0.359f,  0.583f,  0.152f,
		0.483f,  0.596f,  0.789f,
		0.559f,  0.861f,  0.639f,
		0.195f,  0.548f,  0.859f,
		0.014f,  0.184f,  0.576f,
		0.771f,  0.328f,  0.970f,
		0.406f,  0.615f,  0.116f,
		0.676f,  0.977f,  0.133f,
		0.971f,  0.572f,  0.833f,
		0.140f,  0.616f,  0.489f,
		0.997f,  0.513f,  0.064f,
		0.945f,  0.719f,  0.592f,
		0.543f,  0.021f,  0.978f,
		0.279f,  0.317f,  0.505f,
		0.167f,  0.620f,  0.077f,
		0.347f,  0.857f,  0.137f,
		0.055f,  0.953f,  0.042f,
		0.714f,  0.505f,  0.345f,
		0.783f,  0.290f,  0.734f,
		0.722f,  0.645f,  0.174f,
		0.302f,  0.455f,  0.848f,
		0.225f,  0.587f,  0.040f,
		0.517f,  0.713f,  0.338f,
		0.053f,  0.959f,  0.120f,
		0.393f,  0.621f,  0.362f,
		0.673f,  0.211f,  0.457f,
		0.820f,  0.883f,  0.371f,
		0.982f,  0.099f,  0.879f
	};
	GLchar * vertexShaderSource = 	"#version 330 core\n"
					"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
					"layout(location = 1) in vec3 vertexColor;\n"
					"uniform mat4 MVP;\n"
					"out vec3 fragmentColor;\n"
					"void main() {\n"
					"	vec4 v = vec4(vertexPosition_modelspace,1);\n"
					"	gl_Position = MVP * v;\n"
					"	fragmentColor = vertexColor;\n"
					"}";

	GLchar * fragmentShaderSource =	"#version 330\n"
					"in vec3 fragmentColor;\n"
					"out vec3 color;\n"
					"void main() {\n"
					"	color = fragmentColor;\n"
					"}";

	/* Init VAO and VBO */
	initVAO(&VertexArrayID);
	// set vertex buffer
	initVBO(&vertexBuffer, g_vertex_buffer_data);

	// set color buffer
	initVBO(&colorBuffer, g_color_buffer_data);


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
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(0.0f,0.0f,0.0f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
		glUseProgram(ProgramID);

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		render(vertexBuffer,colorBuffer);
		/* WHOA COOL HOW/WHERE THE FUCK SHADERS ARE LINKED WITH VAO?? */

		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);

	glfwTerminate();

	exit(EXIT_SUCCESS);
}
