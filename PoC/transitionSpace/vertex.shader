#version 430

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec2 texCoord;

out vec4 gl_Position;
out vec2 texCoords;

void main() {
  gl_Position = vec4(vertex, 1.0f);
  texCoords = texCoord; // nice naming there huh
}
