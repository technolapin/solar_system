#version 300 es

precision mediump float;

in vec3 vFragColor;

out vec3 fFragColor;

in vec2 vPos;

void main() {

   vec2 P = vPos;

//   float val = length(fract(5.0 * P));
   //float val =length(abs(fract(5.0 * P) * 2.0 - 1.0));
   //float val = mod(floor(10.0 * P.x) + floor(10.0 * P.y), 2.0);
   //float val = smoothstep(0.3, 0.32, length(fract(5.0 * P) - 0.5));
   float val = smoothstep(0.4, 0.5, max(abs(fract(8.0 * P.x - 0.5 * mod(floor(8.0 * P.y), 2.0)) - 0.5), abs(fract(8.0 * P.y) - 0.5)));
  // fFragColor = vFragColor;
   fFragColor = vFragColor*val;
};
