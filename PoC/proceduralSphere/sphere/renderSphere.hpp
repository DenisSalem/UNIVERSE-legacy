#include "../common.hpp"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RenderSphere {
  public:
    RenderSphere(int indexSize, short int * index, int vertexSize, glm::vec3 * vertex[6]);
    void Render(int window_width, int window_height);

  private:
    int indexSize;
    int vertexSize;

    short int * index;

    glm::vec3 ** vertex;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    GLuint  elementBuffer;
    GLuint  VBO[6];
    GLuint  VAO[6];
    GLuint  vertexShaderID;
    GLuint  fragmentShaderID;
    GLuint  programID;
                
    char *  vertexShader;
    char *  fragmentShader;
};
