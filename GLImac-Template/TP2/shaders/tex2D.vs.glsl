#version 300 es


layout(location = 0) in vec2 aVertexPos;
layout(location = 1) in vec2 aVertexUV;

out vec2 vUV;

uniform mat3 uModelMatrix;



void main() {
   vUV = aVertexUV;

   
   gl_Position = vec4(uModelMatrix*vec3(aVertexPos, 1.), 1);
};
