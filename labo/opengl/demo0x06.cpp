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

GLuint loadBMP(const char * imagepath) {
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int width, height;
	unsigned int imageSize;
	unsigned char * data;

	FILE * file = fopen(imagepath,"rb");
	if (!file) {
		std::cout << "Can't open " << imagepath << "\n";
		exit(-1);
	}
	if (fread(header,1,54, file) !=54) {
		std::cout "Not bmp file\n";
		exit(-1);
	}
	if ( header[0]!='B' || header[1]!='M' ){
    		printf("Not a correct BMP file\n");
	 	exit(-1);
	}
	// Read ints from the byte array
	dataPos    = *(int*)&(header[0x0A]);
	imageSize  = *(int*)&(header[0x22]);
	width      = *(int*)&(header[0x12]);
	height     = *(int*)&(header[0x16]);

	// Some BMP files are misformatted, guess missing information
	if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos==0)      dataPos=54; // The BMP header is done that way

	// Create a buffer
	data = new unsigned char [imageSize];
 
	// Read the actual data from the file into the buffer
	fread(data,1,imageSize,file);
 
	//Everything is in memory now, the file can be closed
	fclose(file);

	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);
 
	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
 
	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	return textureID;
}

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
	GLfloat g_uv_buffer_data[] = {
0.000059f, 1.0f-0.000004f,
		0.000103f, 1.0f-0.336048f,
		0.335973f, 1.0f-0.335903f,
		1.000023f, 1.0f-0.000013f,
		0.667979f, 1.0f-0.335851f,
		0.999958f, 1.0f-0.336064f,
		0.667979f, 1.0f-0.335851f,
		0.336024f, 1.0f-0.671877f,
		0.667969f, 1.0f-0.671889f,
		1.000023f, 1.0f-0.000013f,
		0.668104f, 1.0f-0.000013f,
		0.667979f, 1.0f-0.335851f,
		0.000059f, 1.0f-0.000004f,
		0.335973f, 1.0f-0.335903f,
		0.336098f, 1.0f-0.000071f,
		0.667979f, 1.0f-0.335851f,
		0.335973f, 1.0f-0.335903f,
		0.336024f, 1.0f-0.671877f,
		1.000004f, 1.0f-0.671847f,
		0.999958f, 1.0f-0.336064f,
		0.667979f, 1.0f-0.335851f,
		0.668104f, 1.0f-0.000013f,
		0.335973f, 1.0f-0.335903f,
		0.667979f, 1.0f-0.335851f,
		0.335973f, 1.0f-0.335903f,
		0.668104f, 1.0f-0.000013f,
		0.336098f, 1.0f-0.000071f,
		0.000103f, 1.0f-0.336048f,
		0.000004f, 1.0f-0.671870f,
		0.336024f, 1.0f-0.671877f,
		0.000103f, 1.0f-0.336048f,
		0.336024f, 1.0f-0.671877f,
		0.335973f, 1.0f-0.335903f,
		0.667969f, 1.0f-0.671889f,
		1.000004f, 1.0f-0.671847f,
		0.667979f, 1.0f-0.335851f
	};

	GLchar * vertexShaderSource = 	"#version 330 core\n"
					"layout(location = 0) in vec3 vertexPosition_modelspace;\n"
					"layout(location = 1) in vec2 vertexUV;\n"
					"out vec2 UV;\n"
					"uniform mat4 MVP;\n"
					"void main() {\n"
					"	vec4 v = vec4(vertexPosition_modelspace,1);\n"
					"	gl_Position = MVP * v;\n"
					"	UV = vertexUV;\n"
					"}";

	GLchar * fragmentShaderSource =	"#version 330\n"
					"in vec2 UV;\n"
					"out vec3 color;\n"
					"uniform sampler2D myTextureSampler;\n"
					"void main() {\n"
					"	color = texture(myTextureSampler, UV).rgb;\n"
					"}";

	/* Init VAO and VBO */
	initVAO(&VertexArrayID);
	// set vertex buffer
	initVBO(&vertexBuffer, g_vertex_buffer_data);

	GLuint Texture =  loadBMP("");
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
