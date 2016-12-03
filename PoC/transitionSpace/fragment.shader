#version 430

in vec2 texCoords;
out vec4 color;

uniform sampler2D currentTexture;

void main() {
  color = texture(currentTexture, texCoords);
}
