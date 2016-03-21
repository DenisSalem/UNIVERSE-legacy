#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif
#define HEIGHT_MAP_SCALE 256
#define PI 	3.141592


class Planet {
	public:
		Planet(int LOD);
		~Planet();

		static unsigned long int GET_RANDOM();
		void RENDER(int window_width, int window_height);
		
	private:
		void CREATE_MASK_1(float * matrix, unsigned long int scale);
		void CREATE_HEIGHTMAP_1(float * matrix, float * mask, unsigned long int scale, long int offsetX, long int offsetY, unsigned long int realScale);
		void CREATE_VBO();
		void CREATE_VAO();
		void CREATE_ELEMENT_BUFFER();
		void CREATE_SHADER_PROGRAM();
		void LOAD_VERTEX();
		void LOAD_HEIGHTMAP();
		void LOAD_SHADER(char ** vertexShader, const char * fileName);
		void COMPILE_SHADER(GLuint shaderID, char * sourcePointer);
		void CREATE_VERTICES(int LOD);

		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		GLuint VAO;
		GLuint VBO;
		GLuint textureID;
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
		GLuint programID;
		GLuint elementBuffer;
		int vertex_number;
		int indice_number;
		int cube_scale;
		float heightMapMin;
		float heightMapMax;

		glm::vec3 * vertex_array[6];
		short int * indice_array;

		float * heightMap;

		float * mask;

		char * vertexShader;
		char * fragmentShader;
};
