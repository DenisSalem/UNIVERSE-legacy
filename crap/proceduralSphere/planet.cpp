#include <iostream>
#include <cstdio>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "planet.hpp"

Planet::Planet(int LOD) {
	this->model= glm::mat4(1.0);

	this->CREATE_VERTICES(LOD);
	this->CREATE_ELEMENT_BUFFER();
	this->CREATE_VBO();
	this->CREATE_VAO();
	this->LOAD_VERTEX();
	this->CREATE_SHADER_PROGRAM();
}

Planet::~Planet() {
        for (int i=0; i < 6; i++) {
	  glDeleteBuffers(1, &this->VBO[i]);
	  glDeleteBuffers(1, &this->VAO[i]);
        }

        glDeleteBuffers(1, &this->elementBuffer);

        glDetachShader(this->programID, this->vertexShaderID);
	glDetachShader(this->programID, this->fragmentShaderID);

        glDeleteProgram(this->programID);

        glDeleteShader(this->vertexShaderID);
	glDeleteShader(this->fragmentShaderID);

        delete[] this->vertexShader;
	delete[] this->fragmentShader;

        delete[] this->indice_array;

        delete[] this->vertex_array[0];
        delete[] this->vertex_array[1];
        delete[] this->vertex_array[2];
        delete[] this->vertex_array[3];
        delete[] this->vertex_array[4];
        delete[] this->vertex_array[5];
}

void Planet::CREATE_ELEMENT_BUFFER() {
    if(glIsBuffer(this->elementBuffer) == GL_TRUE) {
      glDeleteBuffers(1, &this->elementBuffer);
    }
    glGenBuffers(1, &this->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indice_number * 2 * sizeof(short int), this->indice_array, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Planet::CREATE_VBO() {
    for(int i=0; i<6; i++) {
	if(glIsBuffer(this->VBO[i]) == GL_TRUE) {
        	glDeleteBuffers(1, &this->VBO[i]);
	}

	glGenBuffers(1, &this->VBO[i]);
	glBindBuffer(GL_ARRAY_BUFFER, this->elementBuffer);
    }
}

void Planet::CREATE_VAO() {
    for(int i=0; i<6; i++) {
	if(glIsVertexArray(this->VAO[i]) == GL_TRUE) {
        	glDeleteVertexArrays(1, &this->VAO[i]);
	}
	glGenVertexArrays(1, &this->VAO[i]);
	glBindVertexArray(0);
    }
}

void Planet::LOAD_VERTEX() {
  for(int i=0; i<6; i++) {
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO[i]);
	  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * this->vertex_number, 0, GL_STATIC_DRAW);
	  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * this->vertex_number, this->vertex_array[i]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(this->VAO[i]);
	  glBindBuffer(GL_ARRAY_BUFFER, this->VBO[i]);
	    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	    glEnableVertexAttribArray(0);
	  glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
  }
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
          
          glEnable(GL_CULL_FACE);  
          glPolygonMode(GL_FRONT, GL_LINE);

          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
          for(int i=0; i<6;i++) {
            glBindVertexArray(this->VAO[i]);
              glUniformMatrix4fv(glGetUniformLocation(this->programID, "model"), 1, GL_FALSE, glm::value_ptr(this->model));
              glUniformMatrix4fv(glGetUniformLocation(this->programID, "view"), 1, GL_FALSE, glm::value_ptr(this->view));
              glUniformMatrix4fv(glGetUniformLocation(this->programID, "projection"), 1, GL_FALSE, glm::value_ptr(this->projection));
              glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->elementBuffer);
                glDrawElements(GL_TRIANGLE_STRIP, this->indice_number, GL_UNSIGNED_SHORT, (GLvoid *) (i % 2 == 0 ? 0 : (this->indice_number*2)));
              glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
          }
        glUseProgram(0);
}

void Planet::CREATE_VERTICES(int LOD) {
	this->cube_scale = (2 << (LOD - 1)) + 1;
	float step = 1.0 / (this->cube_scale-1);
	this->vertex_number = pow(this->cube_scale,2);

        // Certaines valeurs serons réutilisés plusieurs fois de suite,
        // on évite donc de répéter chaque calcules ou déréférencement.
        float v1,v2,v3;

        this->indice_number = this->vertex_number*2 - (this->cube_scale + 2);

        std::cout << "Initiate Cube with " << this->vertex_number << " vertices.\n";
	std::cout << "Initiate Cube with " << this->indice_number << " indices.\n";

	this->vertex_array[0] = new glm::vec3[this->vertex_number];
	this->vertex_array[1] = new glm::vec3[this->vertex_number];
	this->vertex_array[2] = new glm::vec3[this->vertex_number];
	this->vertex_array[3] = new glm::vec3[this->vertex_number];
	this->vertex_array[4] = new glm::vec3[this->vertex_number];
	this->vertex_array[5] = new glm::vec3[this->vertex_number];
	this->indice_array = new short int[this->indice_number*2];

        // On peut substantiellement réduire le nombre de calcule en précalculant les rayons de chaques vertex pour une face donnée.
        // On fait ensuite la correspondance entre le rayon du vertex et sa position dans la matrice qui est indépendante de la face
        // sur laquelle on travaille.
        float * radiusPerVertex = new float[this->vertex_number];
	for (int y=0; y<this->cube_scale; y++) {
		for (int x=0; x<this->cube_scale; x++) {
                        v1 = -0.5 + x * step;
                        v2 = 0.5 - y * step;
			radiusPerVertex[this->cube_scale*y + x] = RADIUS / sqrt( pow(v1, 2) +  pow(v2, 2) +  pow(0.5, 2));
                }
        }

	// create vertices
	for (int y=0; y<this->cube_scale; y++) {
		for (int x=0; x<this->cube_scale; x++) {
                        v1 = -0.5 + x * step;
                        v2 = 0.5 - y * step;

                        // Pour éviter de déréférencer 500 milles fois on stock le rayon courant une bonne fois pour toute.
                        v3 = radiusPerVertex[this->cube_scale*y + x];  

			this->vertex_array[0][this->cube_scale*y + x].x = v1 * v3;
			this->vertex_array[0][this->cube_scale*y + x].y = v2 * v3;
			this->vertex_array[0][this->cube_scale*y + x].z = 0.5 * v3;

                        this->vertex_array[1][this->cube_scale*y + x].x = v1 * v3;
			this->vertex_array[1][this->cube_scale*y + x].y = v2 * v3;
			this->vertex_array[1][this->cube_scale*y + x].z = -0.5 * v3;

                        this->vertex_array[2][this->cube_scale*y + x].x = v1 * v3;
			this->vertex_array[2][this->cube_scale*y + x].y = -0.5 * v3;
			this->vertex_array[2][this->cube_scale*y + x].z = v2 * v3;

                        this->vertex_array[3][this->cube_scale*y + x].x = v1 * v3;
			this->vertex_array[3][this->cube_scale*y + x].y = 0.5 * v3;
			this->vertex_array[3][this->cube_scale*y + x].z = v2 * v3;

			this->vertex_array[4][this->cube_scale*y + x].x = 0.5 * v3;
                        this->vertex_array[4][this->cube_scale*y + x].y = v1 * v3;
			this->vertex_array[4][this->cube_scale*y + x].z = v2 * v3;

			this->vertex_array[5][this->cube_scale*y + x].x = -0.5 * v3;
                        this->vertex_array[5][this->cube_scale*y + x].y = v1 * v3;
			this->vertex_array[5][this->cube_scale*y + x].z = v2 * v3;
		}	
	}
	// create indices
	bool reverse = false;

	int x = 0;
	int y = 0;
	int i = 0;
	while (true) {
		this->indice_array[ i ]	= this->cube_scale * y     + x; 
		this->indice_array[ i + 1]   = this->cube_scale * (y+1) + x;
		i+=2;
		if (x == this->cube_scale -1 || (x == 0 && i > 2)) {
			if(y == this->cube_scale -2) {
				break;
			}
			reverse = !reverse;
			y++;
			this->indice_array[ i ]      = this->cube_scale * y + x; 
			this->indice_array[ i + 1]	= this->cube_scale * y + x;
			this->indice_array[ i + 2]	= this->cube_scale *(y+1)+x;
			i+=3;
			
		}
		if (reverse) {
			x--;
		}
		else {
			x++;
		}
	}
        for(int i=0; i < this->indice_number;i++) {
          this->indice_array[this->indice_number + i] = this->indice_array[this->indice_number-1-i];
        }
}
