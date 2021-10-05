#version 330 core

layout (location = 0) in vec2 a_vec;

out vec2 tex_coords;

// uniform mat4 u_translation;
// uniform mat4 u_scale;
uniform mat4 u_model;
uniform mat4 u_projection;

void main ()
{
    tex_coords = 1.0 - a_vec;

    gl_Position = u_projection * u_model * vec4 (a_vec, 0.0, 1.0);
}
