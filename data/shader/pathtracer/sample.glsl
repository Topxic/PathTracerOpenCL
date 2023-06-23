#define PI 3.1415926535897932384626433832795

float rand(vec2 x) {
    return fract(sin(dot(x, vec2(12.9898, 78.233))) * 43758.5453);
}

float randFloat(float d, float idx) {
    float p = rand(gl_GlobalInvocationID.xy);
    p = rand(vec2(p, currentFrame));
    p = rand(vec2(p, time));
    p = rand(vec2(p, d));
    p = rand(vec2(p, idx));
    return p;
}

vec2 sampleDisk() {
    vec3 z = vec3(0);
    float x = randFloat(0, 0);
    float y = randFloat(0, 1);
    y *= 2 * PI;
    return x * vec2(cos(y), sin(y));
}

vec2 sampleStratified(vec2 idx, int n) {
    vec3 z = vec3(0);
    float x = randFloat(idx.x * n, 0);
    float y = randFloat(idx.y * n, 1);
    return vec2((idx.x + x) / n, (idx.y + y) / n);
}

vec3 sampleHemisphere(vec3 n, int numHits, float d, float roughness)
{
    float r1 = randFloat(d, 2 * numHits + 3);
    float r2 = randFloat(r1, 2 * numHits + 1);
    float theta = roughness * roughness * r2 * PI / 2;
    float phi = r1 * 2 * PI;

    float x = sin(theta) * cos(phi);
    float y = cos(theta);
    float z = sin(theta) * sin(phi);

    vec3 tangent;
    if (abs(n.y) < 0.999) {
        tangent = vec3(0, 1, 0);
    } else {
        tangent = vec3(1, 0, 0);
    }
    vec3 bitangent = normalize(cross(n, tangent));
    vec3 result = x * tangent + y * n + z * bitangent;
    return normalize(result);
}








