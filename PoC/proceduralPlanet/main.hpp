#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

#define PI 3.141592

class Planet {
	public:
		Planet(int width, int height, int resolution);
		~Planet();
		void render();
	private:
		int width;
		int height;
	
		void initVBO();
		void initVAO();
		void loadVertex();
		void compileShader(GLuint shaderID, char * sourcePointer);
		void loadShader();
		void initVertex(int iteration);

		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		GLuint VAO;
		GLuint VBO;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
		GLuint programID;
		int vertex_size;
		int vertex_number;
		float * vertex_array;
		char * vertexShader;
		char * fragmentShader;
};
