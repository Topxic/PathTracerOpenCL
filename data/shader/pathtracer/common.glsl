#version 450

layout(std430, binding = 0) buffer renderBuffer {
    vec4 image[];
};

uniform float time;

// Ray marching configuration
uniform vec2 imageResolution;
uniform vec3 cameraPosition;
uniform vec3 cameraDirection;
uniform int maximalRayMarchingSteps;
uniform float maximalRayMarchingDistance;
uniform float epsilon;

// Path tracing configuration
uniform bool calculate;
uniform bool update;
uniform int currentFrame;
uniform int numberOfBounces;

uniform bool automaticFocalLength;
uniform float focalLength;
uniform float aperture;














