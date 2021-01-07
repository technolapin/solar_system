#version 300 es
precision mediump float;

in vec2 vUV;

out vec3 fFragColor;

uniform sampler2D uTexture;

void main()
{
   fFragColor = texture(uTexture, vUV).rgb;
//   fFragColor = vec3(1., 0., 0.);
}

