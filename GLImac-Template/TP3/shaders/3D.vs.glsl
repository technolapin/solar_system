#version 300 es

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormale;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec3 aBitangent;
layout(location = 4) in vec2 aUV;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uMMatrix;
uniform mat4 uNormalMatrix;


out vec3 vNormale;
out vec2 vUV;
out vec3 fPos;
out vec3 fVPos;
out mat3 TBN;

void main() {
   gl_Position = uMVPMatrix * vec4(aPosition, 1.);
   fPos = vec3(uMMatrix * vec4(aPosition, 1.));
   fVPos = vec3(uMVMatrix * vec4(aPosition, 1.));
   vNormale =  vec3((uNormalMatrix* vec4(aNormale, 0.)).xyz);

   vec3 T = normalize(vec3(uMMatrix * vec4(aTangent, 0.0)));
   vec3 B = normalize(vec3(uMMatrix * vec4(aBitangent, 0.0)));
   vec3 N = normalize(vec3(uMMatrix * vec4(aNormale, 0.0)));

   TBN = transpose(mat3(T, B, N));
   
   vUV = aUV;
};
