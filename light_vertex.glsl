#version 330 core
layout (location = 0) in vec4 a_vertex;

out vec2 tex_coords;

uniform mat4 u_model;
uniform mat4 u_projection;

void main ()
{
    tex_coords  = a_vertex.zw;
    gl_Position = vec4 (a_vertex.xy, 0.0, 1.0);
}
