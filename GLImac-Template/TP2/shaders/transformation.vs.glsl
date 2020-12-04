#version 300 es


layout(location = 0) in vec2 aVertexPosition;
layout(location = 1) in vec3 aVertexColor;

out vec3 vFragColor;

out vec2 vPos;


mat3 rotate(float angle)
{
   return mat3(cos(angle) , sin(angle), 0.,
               -sin(angle), cos(angle), 0.,
               0.         , 0.        , 1. );
}

mat3 translate(float x, float y)
{
  return mat3(1., 0., 0.,
              0., 1., 0.,
              x , y , 1.);
}
mat3 deform(float x, float y)
{
  return mat3(x , 0., 0.,
              0., y , 0.,
              0., 0., 1.);
}


void main() {
   vPos = aVertexPosition;
   vFragColor = aVertexColor;

  float angle = 3.14/2.0;
  vec2 translation = vec2(0.5, 0.5);
  float size = 0.6;
  
  mat3 local_rotation = rotate(-1.);
  mat3 global_rotation = rotate(1.);
  
  mat3 trans_mat = translate(0.5, 0.5);

  mat3 resize = deform(1.5, 0.7)*deform(size, size);

  mat3 transformation = size
     *global_rotation
     *trans_mat
     *local_rotation
     *resize;
  vec3 moved = transformation*vec3(aVertexPosition, 1);
  
  gl_Position = vec4(moved.xy/moved.z, 0, 1);
};


