#version 300 es

layout(triangle) in;
layout(points, max_vertices=3) out;

out vec2 vCenter;

void main()
{
   vCenter = (gl_in[0].gl_Position.xy
              + gl_in[1].gl_Position.xy
              + gl_in[2].gl_Position.xy)/3.0;

   gl_Position = gl_in[0].gl_Position
   EmitVertex();
   gl_Position = gl_in[1].gl_Position
   EmitVertex();
   gl_Position = gl_in[2].gl_Position
   EmitVertex();

    EndPrimitive();
}  
