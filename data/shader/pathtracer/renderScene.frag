#include "data/shader/pathtracer/common.glsl"

in vec2 tc;
out vec4 fragColor;

void main() {  
  ivec2 id = ivec2(tc * imageResolution);
  fragColor = image[id.x * int(imageResolution.y) + id.y];
}
