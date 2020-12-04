#version 300 es

layout(triangle) in;
layout(points, max_vertices = 1​) out;

void main()
{
   vec2 pos = gl_in[0].gl_Position
      + gl_in[1].gl_Position
      + gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}  
