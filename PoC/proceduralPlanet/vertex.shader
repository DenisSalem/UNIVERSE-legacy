#version 130

in vec3 vertex;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightMap;

void main() {
	float radius		= 0.7071068;
	float currentRadius	= sqrt( pow(vertex.x, 2) +  pow(vertex.y, 2) +  pow(vertex.z, 2));

	gl_Position = projection * view * model * vec4(vertex.xyz * ((radius) / (currentRadius)), 1.0);
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = vec4(vec3(vertex.x,vertex.y,vertex.z) +0.5, 1.0);
} 
