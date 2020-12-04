#version 300 es

layout(points) in;
layout(points, max_vertices = 1) out;
in vec3 vFragColor[];

out vec3 gFragColor;
void main()
{
    fColor = vColor[0]; // Point has only one vertex
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    EndPrimitive();
}
