#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 800
#define HEIGHT_MAP_SCALE 256
#define PI 	3.141592

int writePng( float * matrix, int size);

unsigned long int getRandom();
void UNIVERSE_MASK_1(float ** matrix, unsigned long int scale);
void UNIVERSE_NOISE_1(float * matrix, float ** mask, unsigned long int scale, long int offsetX, long int offsetY, unsigned long int realScale);

class Planet {
	public:
		Planet(int width, int height, int resolution);
		~Planet();
		void render();
	private:
		float * heightMap;
		float ** mask;

		float heightMapMin;
		float heightMapMax;

		int width;
		int height;
	
		void initTexture();
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
		GLuint textureID;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
		GLuint programID;
		int vertex_size;
		int vertex_number;
		float * vertex_array;
		char * vertexShader;
		char * fragmentShader;
};
