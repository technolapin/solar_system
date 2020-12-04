#version 300 es

precision mediump float;

in vec3 vFragColor;

out vec3 fFragColor;

in vec2 vPos;

void main() {

   float alpha = 10.;
   float beta = 100.0;

   float dist = distance(vPos, vec2(0, 0));
   float attenuation = alpha*exp(-beta*pow(dist, 2.0));

   
   fFragColor = attenuation*vFragColor;
};
