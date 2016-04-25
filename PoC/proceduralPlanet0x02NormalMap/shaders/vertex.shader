#version 130

in vec3 inPosition;

in vec2 inTexcoord;

uniform mat4  model;
uniform mat4  view;
uniform mat4  projection;

out vec2  outTexcoord;

void main() {
	gl_Position = projection * view * model * vec4(inPosition, 1.0);
        outTexcoord =  inTexcoord;
} 
