#version 130

in vec3 vertex;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightMap;

void main() {
	float radius		= 0.7071068;
	float currentRadius	= sqrt( pow(vertex.x, 2) +  pow(vertex.y, 2) +  pow(vertex.z, 2));
	
	float altitude = sqrt( pow(vertex.x,2) + pow(vertex.y,2) + pow(vertex.z,2) );
	float theta = acos(vertex.z/altitude) / 3.141592;
	float phi;

	if (vertex.x >= 0 && vertex.y >= 0)
		phi = atan(vertex.y/vertex.x) / 6.283194;

	if (vertex.x < 0) {
		phi = (1.570796 * 2 + atan(vertex.y/vertex.x) ) / 6.283194;
	}
	else if (vertex.x >= 0 && vertex.y < 0) {
		phi = (6.243184 + atan(vertex.y/vertex.x) ) / 6.283184;
	}

	float depth = texture(heightMap, vec2(phi,theta)).x;
 
	gl_Position = projection * view * model * vec4(vertex.xyz * ((radius) / (currentRadius) - 0.5 * depth) , 1.0);
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = vec4(vec3(vertex.x,vertex.y,vertex.z) + 0.5 - 0.5 * depth, 1.0);
} 
