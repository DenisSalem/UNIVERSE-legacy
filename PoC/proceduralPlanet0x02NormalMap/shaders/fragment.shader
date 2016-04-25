#version 130

in vec2 outTexcoord;
out vec4 color;

uniform sampler2D heightmap;

void main() {
        float step = 0.001953125;
        float scale = 12.5;

        float Z1 = scale * ( texture(heightmap, vec2(outTexcoord.x+step, outTexcoord.y)).r - texture(heightmap, vec2(outTexcoord.x-step, outTexcoord.y)).r );
        float Z2 = scale * ( texture(heightmap, vec2(outTexcoord.x, outTexcoord.y+step)).r - texture(heightmap, vec2(outTexcoord.x, outTexcoord.y-step)).r );

        vec3 A = vec3(1.0,0.0,Z1);
        vec3 B = vec3(0.0,1.0,Z2);

        vec3 Normal = vec3(
          A.y * B.z - A.z * B.y,
          A.x * B.z - A.z * B.z,
          A.x * B.y - A.y * B.x
        );

	color = vec4((Normal+1.0)/2.0, 1.0);
}
