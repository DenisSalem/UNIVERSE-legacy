#include <iostream>
#include <GL/gl3w.h>
#include <cstring>

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
