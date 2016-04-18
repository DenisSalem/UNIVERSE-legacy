#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

#define RADIUS  0.7071068

class Planet {
	public:
		Planet(int LOD);
		~Planet();

		void RENDER(int window_width, int window_height);
		
	private:
		void CREATE_VBO();
		void CREATE_VAO();
		void CREATE_ELEMENT_BUFFER();
		void CREATE_SHADER_PROGRAM();
		void LOAD_VERTEX();
		void LOAD_SHADER(char ** vertexShader, const char * fileName);
		void COMPILE_SHADER(GLuint shaderID, char * sourcePointer);
		void CREATE_VERTICES(int LOD);

		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;

		GLuint VAO[6];
		GLuint VBO[6];
		GLuint vertexShaderID;
		GLuint fragmentShaderID;
		GLuint programID;
		GLuint elementBuffer;

                int vertex_number;
		int indice_number;
		int cube_scale;

                char * vertexShader;
		char * fragmentShader;

		glm::vec3 * vertex_array[6];

                short int * indice_array;
};
