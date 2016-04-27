#version 330

in vec3 vertex;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 inColor;

void main() {
	gl_Position = projection * view * model * vec4(vertex, 1.0);
	inColor = vec4(vertex+0.5, 1.0);
} 
