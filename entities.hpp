#ifndef ENTITIES
#define ENTITIES

#include "body.hpp"
#include "collision.hpp"
#include "shader.hpp"

void add_platform (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = size;
    b.type = PLATFORM;

    push (bodies, b);
}

void add_fire (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = size;
    b.type = FIRE;

    static const vec4 fire_coords[] = {
        { 0, 32, 16, 16 },  { 16, 32, 16, 16 }, { 32, 32, 16, 16 },
        { 48, 32, 16, 16 }, { 64, 32, 16, 16 }, { 80, 32, 16, 16 },
    };

    b.animation = new Animation ();

    *b.animation = {
        0,
        sizeof (fire_coords) / sizeof (vec4),
        { SIMPLE | LOOP, 100, 0, 0, 0 },
        fire_coords,
    };

    push (lights, { b.pos, b.size });
    push (bodies, b);
}

void draw_platform (Body &b)
{
    int   div   = 16;
    float div_x = (int)b.size.x % div;

    while ((int)b.size.x % div != 0)
    {
        if (div == 8)
            break;

        div_x = (int)b.size.x % div;

        div--;
    }

    div_x = div;

    div         = 16;
    float div_y = (int)b.size.y % div;

    while ((int)b.size.y % div != 0)
    {
        if (div == 8)
            break;

        div_y = (int)b.size.y % div;

        div--;
    }

    div_y = div;

    Sprite sprite;

    sprite.size   = { div_x, div_y };
    sprite.angle  = 0;
    sprite.sprite = { 0, 64, 16, 16 };
    sprite.flip   = FLIP_X_TRUE;

    for (int x = b.pos.x; x < b.pos.x + b.size.x; x += div_x)
    {
        // sprite.pos.x = (x - camera.x);

        for (int y = b.pos.y; y < b.pos.y + b.size.y; y += div_y)
        {
            // sprite.pos.y = (y - camera.y);

            // push (sprites, sprite);
        }
    }

    sprite.pos  = b.pos - camera;
    sprite.size = b.size;

    // push (squares, sprite);
    push (squares, { sprite.pos, sprite.size, { 0, .5, .5, 0.2 }, 0, 0 });
}

void draw_fire (Body &b)
{
    push (glows, { b.pos - camera, b.size, { 1.f, .5f, 0, 0.5f } });
    push (sprites, {
                       b.pos - camera,
                       b.size,
                       update (*b.animation),
                       0,
                       FLIP_X_FALSE,
                   });
}

#endif
