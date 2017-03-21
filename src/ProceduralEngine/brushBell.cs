#version 430

layout (local_size_x = 1, local_size_y = 1) in;
layout(r32ui, binding = 0) uniform coherent volatile uimageBuffer bell;

uniform uint brushScale;

float Interpolate(float t) {
  return 6 * pow(t,5) - 15 * pow(t,4) + 10 * pow(t,3);
}

uint GetElevation(uvec2 coord) {
  int halfScale = int(brushScale >> 1);
  int x = int(coord.x - halfScale);
  int y = int(coord.y - halfScale);
  float radius = sqrt(x*x + y*y) / halfScale;
  return uint( (1.0 - Interpolate(radius)) * 4294967295.0);
}

void main() {
  imageStore(bell, int(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y*brushScale*4), uvec4(GetElevation(gl_GlobalInvocationID.xy)));
};
