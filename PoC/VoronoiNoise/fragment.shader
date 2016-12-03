#version 330

in vec2 TexCoord;
out vec4 color;

uniform sampler2D currentTexture;

void main() {
	color = vec4(texture(currentTexture, TexCoord).x);
}
