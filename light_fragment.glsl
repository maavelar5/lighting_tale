#version 330 core

out vec4 color;

in vec2 tex_coords;

uniform int       u_max_lights;
uniform vec2      u_lights[120];
uniform sampler2D u_image;

uniform float u_radius    = 0.05;
uniform float u_intensity = 0.01;
uniform bool  light_room  = true;

struct Light
{
    float intensity, radius;
};

void main ()
{
    vec3 result = texture (u_image, tex_coords).rgb;

    color = vec4 (0, 0, 0, 1.f);

    for (int x = 0; x < u_max_lights; x++)
    {
        float dist
            = distance (tex_coords, vec2 (u_lights[x].x, u_lights[x].y)) / 0.5;

        color += vec4 (u_radius * (result / ((dist * dist) + u_intensity)), 1);
    }

    if (light_room)
    {
        color += vec4 ((u_radius * (result / 0.090)), 1);
    }
}
