#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET
    #define BUFFER_OFFSET(offset) ((char*)NULL + (offset))
#endif

void loadShader(char ** shaderBuffer, const char * fileName) {
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

void compileShader(GLuint shaderID, char * sourcePointer) {
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

int main(int argc, char ** argv) {
  GLFWwindow* window;

  if (!glfwInit())
    exit(-1);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  window = glfwCreateWindow(640, 480, "Simple OpenGL Demo", NULL, NULL);

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
	
  glEnable(GL_DEPTH_TEST);

  // VERTEX
  glm::vec3 * square[2];
    square[0] = new glm::vec3[4];
    square[0][0].x = -0.5;
    square[0][0].y = -0.5;
    square[0][0].z = 0.5;

    square[0][1].x = 0.5;
    square[0][1].y = -0.5;
    square[0][1].z = 0.5;

    square[0][2].x = 0.5;
    square[0][2].y = 0.5;
    square[0][2].z = 0.5;

    square[0][3].x = -0.5;
    square[0][3].y = 0.5;
    square[0][3].z = 0.5;

    square[1] = new glm::vec3[4];
    square[1][0].x = -0.5;
    square[1][0].y = -0.5;
    square[1][0].z = -0.5;

    square[1][1].x = 0.5;
    square[1][1].y = -0.5;
    square[1][1].z = -0.5;

    square[1][2].x = 0.5;
    square[1][2].y = 0.5;
    square[1][2].z = -0.5;

    square[1][3].x = -0.5;
    square[1][3].y = 0.5;
    square[1][3].z = -0.5;

    short int * indices[2];
      indices[0] = new short int[4];
      indices[0][0] = 0;
      indices[0][1] = 3;
      indices[0][2] = 1;
      indices[0][3] = 2;

      indices[1] = new short int[4];
      indices[1][0] = 1;
      indices[1][1] = 2;
      indices[1][2] = 0;
      indices[1][3] = 3;

  // ELEMENT BUFFER
  GLuint elementBuffer[2];
  for (int i = 0; i < 2; i++) {
    if(glIsBuffer(elementBuffer[i]) == GL_TRUE) {
      glDeleteBuffers(1, &elementBuffer[i]);
    }
    glGenBuffers(1, &elementBuffer[i]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer[i]);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(short int), indices[i], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  // CREATE VBO
  GLuint VBO[2];
  for (int i = 0; i < 2; i++) {
    if(glIsBuffer(VBO[i]) == GL_TRUE) {
      glDeleteBuffers(1, &VBO[i]);
    }

    glGenBuffers(1, &VBO[i]);
    glBindBuffer(GL_ARRAY_BUFFER, elementBuffer[i]);
  }     


  // CREATE VAO
  GLuint VAO[2];
  for (int i = 0; i < 2; i++) {
    if(glIsVertexArray(VAO[i]) == GL_TRUE) {
      glDeleteVertexArrays(1, &VAO[i]);
    }

    glGenVertexArrays(1, &VAO[i]);
    glBindVertexArray(0);
  }

  // LOAD VERTEX
  for (int i = 0; i < 2; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
      glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, 0, GL_STATIC_DRAW);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * 4, square[i]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAO[i]);
      glBindBuffer(GL_ARRAY_BUFFER, VBO[i]);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
        glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  // SHADERS
  GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  char * vertexShaderSource;
  char * fragmentShaderSource;

  loadShader(&vertexShaderSource, "vertex.shader");
  loadShader(&fragmentShaderSource, "fragment.shader");

  compileShader(vertexShaderID, vertexShaderSource);
  compileShader(fragmentShaderID, fragmentShaderSource);

  GLuint programID = glCreateProgram();


  glAttachShader(programID, vertexShaderID);
  glAttachShader(programID, fragmentShaderID);

  glLinkProgram(programID);

  GLint Result = GL_FALSE;
  int InfoLogLength = 1024;
  char ProgramErrorMessage[1024] = {0};

  glGetProgramiv(programID, GL_LINK_STATUS, &Result);
  glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  if (strlen(ProgramErrorMessage) != 0) 
    std::cout << ProgramErrorMessage << "\n";

  // RENDER
  glm::mat4 model,view,projection;
  model = glm::mat4(1.0);


  while (!glfwWindowShouldClose(window)) {
    usleep( 40 * 1000 );
    glUseProgram(programID);

      model = glm::rotate(model, (glm::mediump_float) 0.01, glm::vec3(0.0,1.0,0.0));
      view = glm::translate(glm::mat4(1.0), glm::vec3(0.f, 0.0f, -2.00f));
      projection = glm::perspective(45.0, (double) 640/480, 0.1, 10000.0);

      glPolygonMode(GL_FRONT, GL_LINE);
      //glPolygonMode(GL_BACK, GL_LINE);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      
      for (int i=0; i < 2; i++) {
        glBindVertexArray(VAO[i]);
    
          glUniformMatrix4fv(glGetUniformLocation(programID, "model"), 1, GL_FALSE, glm::value_ptr(model));
          glUniformMatrix4fv(glGetUniformLocation(programID, "view"), 1, GL_FALSE, glm::value_ptr(view));
          glUniformMatrix4fv(glGetUniformLocation(programID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

          glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer[i]);
            glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_SHORT, (GLvoid*) 0);

        glBindVertexArray(0);
      }
      glUseProgram(0);
    glfwSwapBuffers(window);
  }
}
