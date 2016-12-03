#include <iostream>
#include <random>
#include <chrono>
#include <functional>
#include <cmath>
#include "dsgl.hpp"
#include "dsglMeshes.hpp"

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <unistd.h>
#endif

const int limit = 4096 / sizeof(unsigned int) - 1; 
const int scale = 512;
const int modScale = 511;
const int depth = 4;
	
int seedsNumber = 16;
int seedOffset = 0;

GLfloat VoronoiCell(int x, int y, unsigned char * seeds) {
	GLfloat tmp, dx, dy;
	
	GLfloat minDist = 16581375.0;
	for (int i = 0; i < seedsNumber; i++) {
		dx = (x - (((int*)seeds)[(seedOffset+i*2) & limit  ] & modScale));
		dy = (y - (((int*)seeds)[(seedOffset+i*2+1) & limit] & modScale));
		tmp = dx*dx+dy*dy;
		if (tmp < minDist) {
			minDist = tmp;
		}
	}

	return sqrt(minDist);
}

GLfloat VoronoiCrack(int x, int y, unsigned char * seeds) {
	GLfloat tmp, dx, dy;
	
	GLfloat minDist1 = 16581375.0;
	GLfloat minDist2 = 16581375.0;
	for (int i = 0; i < seedsNumber; i++) {
		dx = (x - (((int*)seeds)[(seedOffset+i*2) & limit  ] & modScale));
		dy = (y - (((int*)seeds)[(seedOffset+i*2+1) & limit] & modScale));
		tmp = dx*dx+dy*dy;
		if (tmp < minDist2) {
			if (tmp < minDist1) {
			  minDist2 = minDist1;
			  minDist1 = tmp;
			}
			else {
			  minDist2 = tmp;
			}
		}
	}

	return - sqrt(minDist2) + sqrt(minDist1);
}

GLfloat VoronoiJelly(int x, int y, unsigned char * seeds) {
	GLfloat tmp, dx, dy;
	
	GLfloat minDist1 = 16581375.0;
	GLfloat minDist2 = 16581375.0;
	for (int i = 0; i < seedsNumber; i++) {
		dx = (x - (((int*)seeds)[(seedOffset+i*2) & limit  ] & modScale));
		dy = (y - (((int*)seeds)[(seedOffset+i*2+1) & limit] & modScale));
		tmp = dx*dx+dy*dy;
		if (tmp < minDist2) {
			if (tmp < minDist1) {
			  minDist2 = minDist1;
			  minDist1 = tmp;
			}
			else {
			  minDist2 = tmp;
			}
		}
	}

	return -sqrt(minDist2) * sqrt(minDist1);
}


int main(int argc, char ** argv) {
	GLfloat * surface = new GLfloat[scale*scale]();


  	auto time = std::chrono::high_resolution_clock::now();
        auto count = time.time_since_epoch().count();

	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution(0,4294967295);
        generator.seed(std::default_random_engine::result_type(count));
		
	auto dice = std::bind ( distribution, generator );
		
	unsigned char * seeds = new unsigned char[4096];

	for (int i = 0; i < limit; i++) {
		((unsigned int*) seeds)[i] = (unsigned int) dice();
	}

	GLfloat min = 16581375.0;
	GLfloat max = -16581375.0;

	std::chrono::high_resolution_clock::time_point t1,t2;
	t1 = std::chrono::high_resolution_clock::now();	

	GLfloat persistence;

	for (int octave = 1; octave <= depth; octave++) {
	  	seedsNumber *= 4;
		persistence = (1.0 / (GLfloat) (1 << octave));
		for (int x = 0; x < scale; x++) {
			for (int y = 0; y < scale; y++) {
			  	#ifdef VORONOI_CRACK
					surface[x + y * scale] += persistence * VoronoiCrack(x, y, seeds);
				#else
			  		#ifdef VORONOI_JELLY
					surface[x + y * scale] += persistence * VoronoiJelly(x, y, seeds);
					#else
					surface[x + y * scale] += persistence * VoronoiCell(x, y, seeds);
					#endif
				#endif
				if(surface[x + y * scale] < min) {
					min = surface[x + y * scale];
				}
				if(surface[x + y * scale] > max) {
					max = surface[x + y * scale];
				}
			}
		}
		seedOffset += seedsNumber;
	}
	
	t2 = std::chrono::high_resolution_clock::now();	
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>( t2 - t1 ).count();
	std::cout << "Surface duration: " << float(duration) / 1000000.0 << "s\n";

	GLfloat depth;
	for (int x = 0; x < scale; x++) {
		for (int y = 0; y < scale; y++) {
			depth = 1.0 - ((surface[x+scale*y] - min) / (max - min));
			surface[x+scale*y] = depth;
		}
	}

	/* OpenGL context */
	DSGL::Context context("VORONOÏ NOISE WITH DSGL", 512, 512, 4, 4);
	context.InitSimpleWindow();
	
	/* Regular and compute shaders */
	DSGL::PipelineProgram pipelineProgram("vertex.shader","fragment.shader");

	/* Create buffers */
	DSGL::Meshes::Quad quad;

	DSGL::Texture texture(GL_TEXTURE_2D, 512,512, surface, GL_RED, GL_FLOAT, GL_RGBA32F);

	DSGL::Elements elements(4 * sizeof(GLuint), quad.index);
	DSGL::VertexBufferObject VBO(sizeof(GLfloat) * 12, quad.vertex);
	DSGL::VertexBufferObject texCoords(sizeof(GLfloat) * 8, quad.texCoords);
	DSGL::VertexArrayObject VAO(elements.ID, VBO.ID);
	
	/* Set up how vertex related memory is organized */
	VAO.AttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	/* Set up how texcoords is organized in memory */
  	VAO.AttribPointer(texCoords.ID, 1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(GLfloat), 0);

	// ----- Render loop ----- //
	while (!glfwWindowShouldClose(context.window)) {
		glfwPollEvents();
		
		usleep(40000); // Let the GPU take a breath //
    		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// ----- Actual render ----- //
    
		pipelineProgram.Use();
    
		VAO.Bind();
    
		texture.Bind();
    
		glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid *) 0);
    
		texture.Unbind();
            
		glBindVertexArray(0);
    
		glUseProgram(0);

		glfwSwapBuffers(context.window);
	}
	
	delete[] surface;

	return 0;
}
