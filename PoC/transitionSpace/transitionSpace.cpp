#include <iostream>
#include <unistd.h>
#include <glm/glm.hpp>
#include "dsgl.hpp"
#include "dsglMeshes.hpp"

double 	xposOrigin = 0;
double  yposOrigin = 0;
double 	xposDest = 0;
double	yposDest = 0;
GLfloat offsetX=0, offsetY=0;
GLfloat xLatest=-.5, yLatest=-.5;

int mouseState = 0;

DSGL::ShaderProgram * shaderProgramPtr = 0;

// Création du tampon

void CreateStamp(glm::vec4 * stamp) {
	float radius;

        int * powersOfTwo = new int[sizeof(int) * 128];

        // On crée deux tables contenant les valeurs élevées à la puissance de deux.
        // On calcule ainsi n fois ces valeurs au lieu de n².
        for(int x=0; x < 128; x++) {
          powersOfTwo[x] = (x-64) * (x-64);
        }

	for(int x=0; x < 128; x++) { // 16384
		for(int y=0; y < 128; y++) {
                        // On calcule le rayon du cercle sur lequel se trouve le point courant.
                        // Opération très TRÈS gourmante en temps CPU
			radius = sqrtl( powersOfTwo[y] + powersOfTwo[x]);
			if ( radius < 64.0 ) {
                                // y a plus qu'à dessiner le cône.
				stamp[x+y*128].w = (64.0 - radius) / 64.0;
				stamp[x+y*128].x = 1.0;
				stamp[x+y*128].y = 1.0;
				stamp[x+y*128].z = 1.0;
			}
			else {
                                // Si on est en dehors du cercle, on se casse pas la tête et on affecte un zero.
				stamp[x+y*128].w = 0.0;
				stamp[x+y*128].x = 1.0;
				stamp[x+y*128].y = 1.0;
				stamp[x+y*128].z = 1.0;
			}
		}
	}
        delete powersOfTwo;
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		glfwGetCursorPos(window,&xposOrigin,&yposOrigin);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		xLatest += offsetX;
		yLatest += offsetY;
		xLatest =  std::max(-1.0, std::min( (double) (xLatest), 0.0));
		yLatest =  std::max(-1.0, std::min( (double) (yLatest), 0.0));
	}
	offsetX = 0;
	offsetY = 0;
}

void custom_cursor_pos_fun(GLFWwindow * window, double xpos, double ypos) {
	if (1 == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
		glfwGetCursorPos(window,&xposDest,&yposDest);
		offsetX = (xposDest - xposOrigin) / 128.0;
		offsetY = -(yposDest - yposOrigin) / 128.0;
	}
}

int main(int argc, char ** argv) {
	// Création du tampon
	glm::vec4 * stamp = new glm::vec4[16384]; // 128²
	CreateStamp(stamp);
	
	glm::vec4 * cubeCorner = new glm::vec4[65536]; // 256²
	for(int x = 0; x < 256; x++) {
		for(int y = 0; y < 256; y++) {
			if(x < 128 && y < 128) {
				cubeCorner[x + 256 * y].x = 1.0f;
				cubeCorner[x + 256 * y].y = 0.0f;
				cubeCorner[x + 256 * y].z = 0.0f;
				cubeCorner[x + 256 * y].w = 0.25f;
			}
			else if (x < 128 && y >= 128) {
				cubeCorner[x + 256 * y].x = 0.0f;
				cubeCorner[x + 256 * y].y = 1.0f;
				cubeCorner[x + 256 * y].z = 0.0f;
				cubeCorner[x + 256 * y].w = 0.25f;
			}
			else if (x >= 128 && y < 128) {
				cubeCorner[x + 256 * y].x = 0.0f;
				cubeCorner[x + 256 * y].y = 0.0f;
				cubeCorner[x + 256 * y].z = 1.0f;
				cubeCorner[x + 256 * y].w = 0.25f;
			}
			else {
				cubeCorner[x + 256 * y].x = 0.0f;
				cubeCorner[x + 256 * y].y = 0.0f;
				cubeCorner[x + 256 * y].z = 0.0f;
				cubeCorner[x + 256 * y].w = 0.25f;
			}
		}
	}
	// Création d'un quad sur lequel est rendu le tampon
	DSGL::Meshes::Quad quad;

	// Création du context OpenGL
	DSGL::Context context("Transition Space", 512,256, 4,3);
	context.InitSimpleWindow();
	
	// Création des shader program
	DSGL::ShaderProgram cubeCornerShaderProgram("vertex.shader","fragment.shader");

	// Gestion de la souris
	glfwSetMouseButtonCallback(context.window, mouse_button_callback);
	glfwSetCursorPosCallback(context.window, custom_cursor_pos_fun);

	// Création d'une texture contenant une représentation graphique d'un coin d'un cube.
	DSGL::Textures cubeCornerTexture(GL_TEXTURE_2D,256,256, cubeCorner);

	delete[] stamp;
	delete[] cubeCorner;
  
  	// Création du VAO
	DSGL::Elements elements(4 * sizeof(GLuint), quad.index);
	DSGL::VertexBufferObject VBO(sizeof(GLfloat) * 12, quad.vertex);
	DSGL::VertexBufferObject texCoordsBuffer(sizeof(GLfloat) * 8, quad.texCoords);
	DSGL::VertexArrayObject VAO(elements.ID, VBO.ID);
	VAO.AttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0); // Setup VBO
	VAO.AttribPointer(texCoordsBuffer.ID, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0); // Setup TexCoords
  	
	// Création du premier FBO
	DSGL::FrameBufferObject FBO1(256,256);
	
	glEnable(GL_CULL_FACE);
	glEnable (GL_BLEND); glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	
	/* Actual render loop, for instance rely on glfw */
	while (!glfwWindowShouldClose(context.window)) {
		//usleep(10000); /* Let the GPU take a breath */
        	
		glfwSwapBuffers(context.window);
        	
		glfwPollEvents();
		
		glViewport(0,0,256,256);
		
		// Render cubeCorner
		cubeCornerShaderProgram.Use();
		VAO.Bind();
		cubeCornerTexture.Bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid *) 0);
		cubeCornerTexture.Unbind();
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glUseProgram(0);
	}

	return DSGL_END_NICELY;	
}
