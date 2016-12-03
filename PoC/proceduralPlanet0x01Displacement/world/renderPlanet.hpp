#include "../common.hpp"
#include "planetFactory.hpp"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class RenderPlanet {
  public:
    RenderPlanet(PlanetFactory * planet);
    void Render(int window_width, int window_height);

  private:
    PlanetFactory * planet;
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
