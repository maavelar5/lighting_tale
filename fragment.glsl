#version 330 core

in vec2 tex_coords;

out vec4 color;

uniform vec4      u_offset;
uniform sampler2D u_image;

uniform bool u_flip_x = false;
uniform bool u_flip_y = true;

uniform int  u_type  = 0;
uniform vec4 u_color = vec4 (0, 0, 0, 1);

uniform float u_radius    = 0.15;    //   = 0.2;
uniform float u_intensity = 0.01;    //= 0.07;

uniform float u_alpha = 0.5;

void main ()
{
    if (u_type == 0)
    {
        vec2 t = tex_coords;

        if (u_flip_x)
        {
            t.x = 1.0f - t.x;
        }

        if (u_flip_y)
        {
            t.y = 1.0f - t.y;
        }

        color = texture (u_image, t * u_offset.zw + u_offset.xy);
    }
    else if (u_type == 1)
    {
        color = u_color;
    }
    else if (u_type == 2)
    {
        float dist = distance (tex_coords, vec2 (.5, .5)) / u_radius;

        color = vec4 (u_intensity * (u_color.xyz / ((dist * dist) + 0.01)),
                      u_alpha);
    }
}
