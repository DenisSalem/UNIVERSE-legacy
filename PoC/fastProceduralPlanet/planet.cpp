#include <iostream>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planet.hpp"

Planet::Planet(int LOD) {
	// Define first heightmap rendering max and min height.
	this->heightMapMax = 0.75;
	this->heightMapMin = 0.125;
	
	//We emulate matrix with regular array
	this->heightMap = new float[ HEIGHT_MAP_SCALE * HEIGHT_MAP_SCALE ];
	this->mask = new float[ HEIGHT_MAP_SCALE * HEIGHT_MAP_SCALE ];


	// Clear and initiate both heightmap and mask.
	for(int x = 0; x < HEIGHT_MAP_SCALE; x++) {
		for(int y = 0; y < HEIGHT_MAP_SCALE; y++) {
			this->heightMap[x * HEIGHT_MAP_SCALE + y] = 0;
		}
	}


	this->CREATE_MASK_1(this->mask, HEIGHT_MAP_SCALE);
	this->CREATE_HEIGHTMAP_1(this->heightMap, this->mask, HEIGHT_MAP_SCALE, 0, 0, HEIGHT_MAP_SCALE);
	

	// Values of the heightmap need to be clamped to the range [0,1]
	float max=0,min = 65536;
	for (int x = 0; x <  HEIGHT_MAP_SCALE; x++) {
		for (int y = 0; y <  HEIGHT_MAP_SCALE; y++) {
			if ( this->heightMap[x * HEIGHT_MAP_SCALE + y ] > max) {
				max = this->heightMap[x * HEIGHT_MAP_SCALE + y];
			}
			if ( this->heightMap[x * HEIGHT_MAP_SCALE + y ] < min) {
				min = this->heightMap[x * HEIGHT_MAP_SCALE + y];
			}
		}
	}

	// Because it's the first heightmap generated, we need to adjust levels.
	for (int x = 0; x <  HEIGHT_MAP_SCALE; x++) {
		for (int y = 0; y <  HEIGHT_MAP_SCALE; y++) {
			this->heightMap[x * HEIGHT_MAP_SCALE + y] = (this->heightMap[x * HEIGHT_MAP_SCALE + y]) / max;
			if (this->heightMap[x * HEIGHT_MAP_SCALE + y] < max * this->heightMapMin) {
				this->heightMap[x * HEIGHT_MAP_SCALE + y] = max * this->heightMapMin;
			}
			if (this->heightMap[x * HEIGHT_MAP_SCALE + y] > max - this->heightMapMax * max) {
				this->heightMap[x * HEIGHT_MAP_SCALE + y] = max - this->heightMapMax * max;	
			}
		}
	}
	
	this->model= glm::mat4(1.0);

	this->CREATE_VERTICES(LOD);
	this->CREATE_ELEMENT_BUFFER();
	this->LOAD_HEIGHTMAP();
	this->CREATE_VBO();
	this->CREATE_VAO();
	this->LOAD_VERTEX();
	this->CREATE_SHADER_PROGRAM();
}

unsigned long int Planet::GET_RANDOM() {
        timespec tStruct;
        clock_gettime(CLOCK_REALTIME, &tStruct);
        return tStruct.tv_nsec;
}

void Planet::CREATE_MASK_1(float * mask, unsigned long int scale) {
        unsigned long int x,y;
        long double halfScale = scale / 2 ;
        long double radius;
        for(x=0;x<scale;x++) {
                for(y=0;y<scale;y++) {
                        radius = sqrtl( ((y-halfScale) * (y-halfScale)) + ((x-halfScale) * (x-halfScale)));
                        if ( radius < halfScale ) {
                                mask[ x * scale + y ] = (halfScale - radius) / (halfScale);
                        }
                        else {
                                mask[ x*scale +y ] = 0;
                        }
                }
        }
}

void Planet::CREATE_HEIGHTMAP_1(float * matrix, float * mask, unsigned long int scale, long int offsetX, long int offsetY, unsigned long int realScale) {
        if (scale == 1) {
                return;
        }
        long int halfScale = scale >> 1; 
        long int x, y; 
        long int randX = - halfScale + Planet::GET_RANDOM() % scale;
        long int randY = - halfScale + Planet::GET_RANDOM() % scale;
        int inc = realScale / scale;
        unsigned long int maskX=0, maskY=0;
        long int wrapX,wrapY;
        char doIt = Planet::GET_RANDOM() & 1; 
        if (doIt) {
        for(x=0;x<scale;x++) {
                maskY = 0; 
                        for(y=0;y<scale;y++) {
                                // Easy Case
                                if ( randX + offsetX + x < realScale && randX + offsetX +x >= 0 && randY + offsetY + y < realScale && randY + offsetY + y >= 0) {
                                        matrix[ ((x+randX+offsetX) * realScale) + y+randY+offsetY] += mask[maskX * scale + maskY] / (inc);
                                }
                                // Wrap shit n stuff
                                else {
                                        wrapX = 0; 
                                        wrapY = 0; 

                                        if ( randX + offsetX + x >= realScale && randX + offsetX + x < realScale*2 ) {
                                                wrapX = - realScale;
                                        }
                                        else if ( randX + offsetX + x > -realScale && randX + offsetX + x < 0) {
                                                wrapX = realScale;
                                        }

                                        if ( randY + offsetY + y > -realScale && randY + offsetY + y < 0) {
                                                wrapY = realScale;
                                        }
                                        else if ( randY + offsetY + y < realScale * 2 && randY + offsetY + y >= realScale) {
                                                wrapY = -realScale;
                                        }

                                        matrix[ (  wrapX + x + offsetX  + randX) * realScale + (y+randY+offsetY) + wrapY] += mask[maskX * scale + maskY] / (inc);
                                }
                                maskY+=inc;
                        }
                        maskX+=inc;
                }
        }
        CREATE_HEIGHTMAP_1(matrix, mask, scale/2, offsetX+0, offsetY+0, realScale);
        CREATE_HEIGHTMAP_1(matrix, mask, scale/2, offsetX+0, offsetY+scale/2, realScale);
        CREATE_HEIGHTMAP_1(matrix, mask, scale/2, offsetX+scale/2, offsetY+scale/2, realScale);
        CREATE_HEIGHTMAP_1(matrix, mask, scale/2, offsetX+scale/2, offsetY+0, realScale);
}


Planet::~Planet() {
	glDeleteBuffers(1, &this->VBO);
	glDeleteBuffers(1, &this->VAO);
	glDeleteBuffers(1, &this->elementBuffer);
	glDeleteBuffers(1, &this->textureID);
	glDetachShader(this->programID, this->vertexShaderID);
	glDetachShader(this->programID, this->fragmentShaderID);
	glDeleteProgram(this->programID);
	glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);
	delete[] this->vertexShader;
	delete[] this->fragmentShader;
}

void Planet::LOAD_HEIGHTMAP() {
	glGenTextures(1, &this->textureID);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
		glTexImage2D(GL_TEXTURE_2D, 0,  GL_R32F, HEIGHT_MAP_SCALE, HEIGHT_MAP_SCALE, 0, GL_RED, GL_FLOAT, this->heightMap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D,0);
}

void Planet::CREATE_ELEMENT_BUFFER() {
	if(glIsBuffer(this->elementBuffer) == GL_TRUE) {
        	glDeleteBuffers(1, &this->elementBuffer);
	}
	glGenBuffers(1, &this->elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indice_number * sizeof(unsigned short), this->indice_array, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Planet::CREATE_VBO() {
	if(glIsBuffer(this->VBO) == GL_TRUE) {
        	glDeleteBuffers(1, &this->VBO);
	}

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, this->elementBuffer);
}

void Planet::CREATE_VAO() {
	if(glIsVertexArray(this->VAO) == GL_TRUE) {
        	glDeleteVertexArrays(1, &this->VAO);
	}
	glGenVertexArrays(1, &this->VAO);
	glBindVertexArray(0);
}

void Planet::LOAD_VERTEX() {
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertex_number, 0, GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * this->vertex_number, this->vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
			glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Planet::LOAD_SHADER(char ** shaderBuffer, const char * fileName) {
	FILE * shader = fopen (fileName, "r");
	(*shaderBuffer) = new char[1280];
	for (int i=0;i<1280;i++) {
		(*shaderBuffer)[i] = (unsigned char ) fgetc(shader);
		if ((*shaderBuffer)[i] == EOF) {
			(*shaderBuffer)[i] = '\0';
			break;
		}
	}
	
	fclose(shader);
}

void Planet::COMPILE_SHADER(GLuint shaderID, char * sourcePointer) {
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

void Planet::CREATE_SHADER_PROGRAM() {
	this->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	this->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	this->LOAD_SHADER(&this->vertexShader, "vertex.shader");
	this->LOAD_SHADER(&this->fragmentShader, "fragment.shader");

	this->COMPILE_SHADER(this->vertexShaderID, this->vertexShader);
	this->COMPILE_SHADER(this->fragmentShaderID, this->fragmentShader);

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

void Planet::RENDER(int window_width, int window_height) {
        glUseProgram(this->programID);

        this->model = glm::rotate(this->model, (glm::mediump_float) 0.01, glm::vec3(1.0,1.0,1.0));
        this->view = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.0f, -2.00f));
        this->projection = glm::perspective(45.0, (double) window_width/window_height, 0.1, 10000.0);

        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(this->VAO);

                glUniformMatrix4fv(glGetUniformLocation(this->programID, "model"), 1, GL_FALSE, glm::value_ptr(this->model));
                glUniformMatrix4fv(glGetUniformLocation(this->programID, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
                glUniformMatrix4fv(glGetUniformLocation(this->programID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));

                glBindTexture(GL_TEXTURE_2D, this->textureID);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
		glDrawElements(GL_TRIANGLE_STRIP, this->indice_number, GL_UNSIGNED_SHORT, (GLvoid*)0);
        glBindVertexArray(0);
        glUseProgram(0);
}

void Planet::CREATE_VERTICES(int LOD) {
	this->cube_scale = (2 << (LOD - 1)) + 1;
	float step = 1.0 / (this->cube_scale-1);
	this->vertex_number = pow(this->cube_scale,2);
	this->indice_number = this->vertex_number*2 - 2 * this->cube_scale;
	std::cout << "Initiate Cube with " << this->vertex_number << " vertices.\n";
	std::cout << "Initiate Cube with " << this->indice_number << " indices.\n";

	this->vertex_array = new glm::vec3[this->vertex_number];
	this->indice_array = new short int[this->indice_number];

	// create vertices
	for (int y=0; y<this->cube_scale; y++) {
		for (int x=0; x<this->cube_scale; x++) {
			this->vertex_array[this->cube_scale*y + x].x = -0.5 + x * step;
			this->vertex_array[this->cube_scale*y + x].y = 0.5 - y * step;
			this->vertex_array[this->cube_scale*y + x].z = 0.5;
		}	
	}

	// create indices
	int offset= 18;
	bool reverse = false;

	int x = 0;
	int y = 0;
	int i = 0;

	while (true) {
		this->indice_array[ i ]		= this->cube_scale * y + x; 
		this->indice_array[ i + 1]	= this->cube_scale*(y+1)+x;
		i+=2;
		if (x == this->cube_scale -1 || (x == 0 && i > 2)) {
			if(y == this->cube_scale -2) {
				break;
			}
			reverse = !reverse;
			y++;
			this->indice_array[ i ]		= this->cube_scale * y + x; 
			this->indice_array[ i + 1]	= this->cube_scale *(y+1)+x;
			i+=2;
			
		}
		if (reverse) {
			x--;
		}
		else {
			x++;
		}
	}
}
