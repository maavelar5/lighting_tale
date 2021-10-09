#ifndef PLAYER_INCL
#define PLAYER_INCL

#include "body.hpp"
#include "collision.hpp"
#include "controller.hpp"
#include "globals.hpp"
#include "shader.hpp"
#include "time_data.hpp"
#include "timer.hpp"

void init_player ()
{
    Body b = get_body ();

    b.pos    = { 160.f, 160.f };
    b.size   = { 16.f, 16.f };
    b.type   = PLAYER;
    b.config = COLLISION_SPACING;
    b.speed  = 200.f;

    push (bodies, b);
}

void move_player (Body &b)
{
    b.prev = b.pos;

    if (just_pressed (controller.inputs, PLAYER_JUMP))
    {
        if (b.sensor & (LEFT | RIGHT) && !(b.sensor & BOT))
        {
            just_pressed (controller.inputs, PLAYER_JUMP, true);

            b.vel.x = (b.vel.x > 0) ? -b.speed : b.speed;
            b.vel.y = -250.f;
        }
        else if (b.sensor & BOT)
        {
            just_pressed (controller.inputs, PLAYER_JUMP, true);
            b.vel.y = -250.f;
        }
    }

    if (controller.state & (PLAYER_LEFT | PLAYER_RIGHT))
    {
        if (controller.state & PLAYER_LEFT)
        {
            b.accel.x = (b.vel.x > 0) ? -b.speed * 8 : -b.speed;

            if (b.vel.x < -b.speed)
                b.accel.x = 0;
        }
        else if (controller.state & PLAYER_RIGHT)
        {
            b.accel.x = (b.vel.x < 0) ? b.speed * 8 : b.speed;

            if (b.vel.x > b.speed)
                b.accel.x = 0;
        }

        b.vel.x += (b.accel.x * time_data::step);
    }
    else
    {
        if (b.vel.x > 0)
            b.accel.x = -b.speed * 8;
        else if (b.vel.x < 0)
            b.accel.x = b.speed * 8;

        b.vel.x += (b.accel.x * time_data::step);

        if ((b.accel.x > 0 && b.vel.x > 0) || (b.accel.x < 0 && b.vel.x < 0))
            b.vel.x = 0;
    }

    // if (!(b.sensor & BOT))
    // {
    float local_gravity = gravity;
    float max_gravity   = 400.f;

    if (b.vel.y > 0)
    {
        if (b.sensor & LEFT && controller.state & PLAYER_LEFT)
            max_gravity /= 8.f;
        else if (b.sensor & RIGHT && controller.state & PLAYER_RIGHT)
            max_gravity /= 8.f;
    }

    b.vel.y += (local_gravity * time_data::step);

    if (b.vel.y > max_gravity)
        b.vel.y = max_gravity;
    // }

    b.pos += (b.vel * time_data::step);

    if ((b.accel.x > 0 || b.vel.x > 0) && b.pos.x - camera.x > ((W / 3) * 2))
    {
        camera.x += abs (b.pos.x - b.prev.x);
    }
    else if ((b.accel.x < 0 || b.vel.x < 0) && b.pos.x - camera.x < (W / 3))
    {
        camera.x -= abs (b.pos.x - b.prev.x);
    }

    if (b.vel.y > 0 && b.pos.y - camera.y > ((H / 3) * 2.3)
        && !(b.sensor & BOT))
    {
        camera.y += abs (b.pos.y - b.prev.y);
    }
    else if (b.vel.y < 0 && b.pos.y - camera.y < (H / 3) && !(b.sensor & TOP))
    {
        camera.y -= abs (b.pos.y - b.prev.y);
    }

    if (camera.y < 0)
        camera.y = 0;
    if (camera.x < 0)
        camera.x = 0;

    b.sensor = NONE;

    push (grid, &b);
}

void draw_player (Body &b)
{
    static vec4 standing_coords[] = {
        { 0, 0, 16, 16 },
        { 16, 0, 16, 16 },
        { 32, 0, 16, 16 },
    };

    static Animation standing = {
        0,
        3,
        { SIMPLE | LOOP, 2000, 0, 0, 0 },
        standing_coords,
    };

    static vec4 walking_coords[] = {
        { 48, 0, 16, 16 },
        { 64, 0, 16, 16 },
        { 80, 0, 16, 16 },
    };

    static Animation walking = {
        0,
        3,
        { SIMPLE | LOOP, 150, 0, 0, 0 },
        walking_coords,
    };

    static vec4 jumping_coords[] = {
        { 96, 0, 16, 16 },
    };

    static Animation jumping = {
        0,
        1,
        { SIMPLE | LOOP, 100, 0, 0, 0 },
        jumping_coords,
    };

    static vec4 hold_coords[] = {
        { 128, 0, 16, 16 },
    };

    static Animation hold = {
        0,
        1,
        { SIMPLE | LOOP, 100, 0, 0, 0 },
        hold_coords,
    };

    static Light *light = &push (lights, { b.pos, b.size });

    light->pos  = b.pos;
    light->size = b.size;

    static uint flip = NONE;

    vec4       sprite;
    Animation *animation = 0;

    if (controller.state & PLAYER_LEFT)
        flip = FLIP_X_FALSE;
    else if (controller.state & PLAYER_RIGHT)
        flip = FLIP_X_TRUE;

    if (b.sensor & (LEFT | RIGHT))
        animation = &hold;
    if (!(b.sensor & BOT))
        animation = &jumping;
    else if (controller.state & (PLAYER_LEFT | PLAYER_RIGHT))
        animation = &walking;
    else
        animation = &standing;

    sprite = update (*animation);

    push (glows, { b.pos - camera, b.size, { 0.0f, 0.5f, .5f, 0.5f } });
    push (sprites, { b.pos - camera, b.size, sprite, b.angle, flip });
}

#endif
