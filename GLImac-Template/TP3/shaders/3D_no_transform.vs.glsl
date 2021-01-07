#version 300 es

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormale;
layout(location = 2) in vec2 aUV;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uMMatrix;
uniform mat4 uNormalMatrix;


out vec2 vUV;


void main() {
   mat4 P = uMVPMatrix * inverse(uMVMatrix);
   mat4 V = uMVMatrix * inverse(uMMatrix);
   mat4 M = uMMatrix;

   V[3] = vec4(0, 0, 0, 1); 

   mat4 mat = P*V*M;
   gl_Position = mat * vec4(aPosition, 1.);
};
