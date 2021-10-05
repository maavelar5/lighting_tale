#version 330 core

in vec2 tex_coords;

out vec4 color;

uniform vec4      u_offset;
uniform sampler2D u_image;

uniform bool u_flip_x = false;
uniform bool u_flip_y = true;

uniform int  u_type  = 0;
uniform vec4 u_color = vec4 (0, 0, 0, 1);

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
}
