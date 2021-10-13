#ifndef ENTITIES
#define ENTITIES

#include "body.hpp"
#include "collision.hpp"
#include "shader.hpp"

enum PLAYING_ACTIONS
{
    PLAYER_JUMP  = 1,
    PLAYER_LEFT  = 2,
    PLAYER_RIGHT = 4,
    PLAYER_DASH  = 8,

    SHOW_EDITOR            = 16,
    TOGGLE_ASPECT_RATIO    = 32,
    TOGGLE_COLLISION_BOXES = 64,
};

void add_platform (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = size;
    b.type = PLATFORM;

    push (&bodies, b);
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

    push (&lights, (Light) { b.pos, b.size, true });
    push (&bodies, b);
}

void draw_platform (Body &b)
{
    Sprite sprite;

    sprite.angle  = 0;
    sprite.sprite = { 0, 64, 16, 16 };
    sprite.flip   = FLIP_X_TRUE;

    sprite.pos  = b.pos - camera;
    sprite.size = b.size;

    push (&squares,
          (Square) { sprite.pos, sprite.size, { 0, .5, .5, 0.2 }, 0, 0 });
}

void draw_fire (Body &b)
{
    push (&glows, { b.pos - camera, b.size, { 1.f, .5f, 0, 0.5f } });
    push (&sprites, {
                        b.pos - camera,
                        b.size,
                        update (*b.animation),
                        0,
                        FLIP_X_FALSE,
                    });
}

Body *add_player (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos    = pos;
    b.size   = size;
    b.type   = PLAYER;
    b.config = COLLISION_SPACING;
    b.speed  = 200.f;
    b.accel  = { 0, 0 };

    return push (&bodies, b);
}

void move_player (Body *b, Controller *controller)
{
    b->prev_pos = b->pos;

    if (just_pressed (controller->inputs, PLAYER_JUMP))
    {
        if (b->sensor & (LEFT | RIGHT) && !(b->sensor & BOT))
        {
            just_pressed (controller->inputs, PLAYER_JUMP, true);

            b->vel.x = (b->vel.x > 0) ? -b->speed : b->speed;
            b->vel.y = -250.f;
        }
        else if (b->sensor & BOT)
        {
            just_pressed (controller->inputs, PLAYER_JUMP, true);
            b->vel.y = -250.f;
        }
    }

    if (controller->state & (PLAYER_LEFT | PLAYER_RIGHT))
    {
        if (controller->state & PLAYER_LEFT)
        {
            b->accel.x = (b->vel.x > 0) ? -b->speed * 8 : -b->speed;

            if (b->vel.x < -b->speed)
                b->accel.x = 0;
        }
        else if (controller->state & PLAYER_RIGHT)
        {
            b->accel.x = (b->vel.x < 0) ? b->speed * 8 : b->speed;

            if (b->vel.x > b->speed)
                b->accel.x = 0;
        }

        b->vel.x += (b->accel.x * time_data::step);
    }
    else
    {
        if (b->vel.x > 0)
            b->accel.x = -b->speed * 8;
        else if (b->vel.x < 0)
            b->accel.x = b->speed * 8;

        b->vel.x += (b->accel.x * time_data::step);

        if ((b->accel.x > 0 && b->vel.x > 0)
            || (b->accel.x < 0 && b->vel.x < 0))
            b->vel.x = 0;
    }

    if (!(b->sensor & BOT))
    {
        float local_gravity = gravity;
        float max_gravity   = 400.f;

        if (b->vel.y > 0)
        {
            if (b->sensor & LEFT && controller->state & PLAYER_LEFT)
                max_gravity /= 8.f;
            else if (b->sensor & RIGHT && controller->state & PLAYER_RIGHT)
                max_gravity /= 8.f;
        }

        b->vel.y += (local_gravity * time_data::step);

        if (b->vel.y > max_gravity)
            b->vel.y = max_gravity;
    }

    b->pos += (b->vel * time_data::step);

    if ((b->accel.x > 0 || b->vel.x > 0) && b->pos.x - camera.x > ((W / 3) * 2))
    {
        camera.x += fabs (b->pos.x - b->prev_pos.x);
    }
    else if ((b->accel.x < 0 || b->vel.x < 0) && b->pos.x - camera.x < (W / 3))
    {
        camera.x -= fabs (b->pos.x - b->prev_pos.x);
    }

    if (b->vel.y > 0 && b->pos.y - camera.y > ((H / 3) * 2.3)
        && !(b->sensor & BOT))
    {
        camera.y += fabs (b->pos.y - b->prev_pos.y);
    }
    else if (b->vel.y < 0 && b->pos.y - camera.y < (H / 3)
             && !(b->sensor & TOP))
    {
        camera.y -= fabs (b->pos.y - b->prev_pos.y);
    }

    if (camera.y < 0)
        camera.y = 0;
    if (camera.x < 0)
        camera.x = 0;

    b->sensor = NONE;

    push (&grid, b);
}

void draw_player (Body &b, Controller *controller)
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

    static Light *light = push (&lights, { b.pos, b.size, true });

    light->pos  = b.pos;
    light->size = b.size;

    static uint flip = NONE;

    vec4       sprite;
    Animation *animation = 0;

    if (controller->state & PLAYER_LEFT)
        flip = FLIP_X_FALSE;
    else if (controller->state & PLAYER_RIGHT)
        flip = FLIP_X_TRUE;

    if (b.sensor & (LEFT | RIGHT))
        animation = &hold;
    else if (!(b.sensor & BOT))
        animation = &jumping;
    else if (controller->state & (PLAYER_LEFT | PLAYER_RIGHT))
        animation = &walking;
    else
        animation = &standing;

    sprite = update (*animation);

    push (&glows, (Glow) { b.pos - camera, b.size, { 0.0f, 0.5f, .5f, 0.5f } });
    push (&sprites, (Sprite) { b.pos - camera, b.size, sprite, b.angle, flip });
}

#endif
