#include <cstdio>

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
