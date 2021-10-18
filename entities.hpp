#ifndef ENTITIES
#define ENTITIES

#include "body.hpp"
#include "collision.hpp"
#include "shader.hpp"

inline void add_platform (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = size;
    b.type = PLATFORM;

    push (&bodies, b);
}

inline void add_enemy (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = size;
    b.type = ENEMY;

    b.vel = { 0.f, 300.f };

    push (&bodies, b);
}

inline void add_fire (vec2 pos, vec2 size)
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

    push (&bodies, b);
}

inline void draw_platform (Body &b)
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

inline void draw_fire (Body &b)
{
    push (&glows, { b.pos - camera, b.size, { 1.f, .5f, 0, 0.5f } });
    push (&lights, { b.pos, b.size, true });

    push (&sprites, {
                        b.pos - camera,
                        b.size,
                        update (b.animation),
                        0,
                        FLIP_X_FALSE,
                        1,
                    });
}

inline void draw_enemy (Body &b)
{
    // push (&glows, { b.pos - camera, b.size, { 1.f, 1.f, 0.0f } });

    push (&sprites, { b.pos - camera,
                      b.size,
                      { 0.f, 16.f, 16.f, 16.f },
                      SDL_GetTicks () / 3.f,
                      FLIP_X_FALSE,
                      1 });
}

inline void add_player (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos   = pos;
    b.size  = size;
    b.type  = PLAYER;
    b.speed = 200.f;
    b.accel = { 0, 0 };

    Body *player = push (&bodies, b);

    player->player = (Player *)malloc (sizeof (Player));

    player->player->flip        = NONE;
    player->player->sword_delay = { TWO_WAY, 200, 250, 0, DONE };

    b.type = PLAYER_SWORD;
    b.size = { 8.f, 16.f };
    b.pos  = b.pos;

    player->player->sword = push (&bodies, b);
}

inline void move_enemy (Body *b)
{
    float speed = 250.f;

    if (b->sensor)
    {
        if (b->sensor & TOP)
        {
            b->vel.y = speed;
        }
        if (b->sensor & BOT)
        {
            b->vel.y = -speed;
        }
        if (b->sensor & LEFT)
        {
            b->vel.x = speed;
        }
        if (b->sensor & RIGHT)
        {
            b->vel.x = -speed;
        }
    }

    b->vel.y += ((gravity / 8) * time_data::step);

    b->pos += (b->vel * time_data::step);

    b->sensor = NONE;

    push (&grid, b);
}

inline void move_player (Body *b, Inputs *inputs)
{
    b->prev_pos = b->pos;

    Input *left   = 0;
    Input *jump   = 0;
    Input *right  = 0;
    Input *attack = 0;

    Body *sword = b->player->sword;

    for (Input *i = inputs->first; i != limit (*inputs); i = i->next)
    {
        switch (i->action)
        {
            case PLAYER_JUMP: jump = i; break;
            case PLAYER_LEFT: left = i; break;
            case PLAYER_RIGHT: right = i; break;
            case PLAYER_ATTACK: attack = i; break;
        }
    }

    if (active (jump))
    {
        if (b->sensor & (LEFT | RIGHT) && !(b->sensor & BOT))
        {
            set (&jump->timer, DONE);

            b->vel.x = (b->vel.x > 0) ? -b->speed : b->speed;
            b->vel.y = -250.f;
        }
        else if (b->sensor & BOT)
        {
            set (&jump->timer, DONE);
            b->vel.y = -250.f;
        }
    }

    if (left || right)
    {
        if (left)
        {
            b->accel.x = (b->vel.x > 0) ? -b->speed * 8 : -b->speed;

            if (b->vel.x < -b->speed)
                b->accel.x = 0;
        }
        else if (right)
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
            if (b->sensor & LEFT && left)
                max_gravity /= 8.f;
            else if (b->sensor & RIGHT && right)
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

    if (b->vel.y > 0 && b->pos.y - camera.y > ((H / 3) * 2)
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

    sword->pos = b->pos;

    if (active (attack) && b->player->sword_delay.state & (DONE))
        set (&b->player->sword_delay, START | JUST_STARTED);

    if (b->player->sword_delay.state & START)
        sword->size = { 24.f, 32.f };
    else
        sword->size = { 8.f, 16.f };

    sword->pos.y -= (sword->size.y - b->size.y) / 2.f;

    if (b->player->flip == FLIP_X_TRUE)
        sword->pos.x += b->size.x;
    else
        sword->pos.x -= sword->size.x;

    push (&grid, b);

    if (b->player->sword_delay.state & START)
    {
        push (&grid, b->player->sword);
    }

    update (&b->player->sword_delay);
}

inline void draw_player (Body &b, Inputs *inputs)
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

    static vec4 sword_coords[] = {
        { 16, 80, 16, 16 },
        { 16, 80, 16, 16 },
        { 32, 80, 16, 16 },
        { 48, 80, 16, 16 },
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

    static Animation sword = {
        0,
        4,
        { SIMPLE | LOOP, 75, 0, 0, 0 },
        sword_coords,
    };

    uint *flip = &b.player->flip;

    vec4       sprite;
    Animation *animation = 0;

    Input *left  = 0;
    Input *right = 0;

    for (Input *i = inputs->first; i != limit (*inputs); i = i->next)
    {
        switch (i->action)
        {
            case PLAYER_LEFT: left = i; break;
            case PLAYER_RIGHT: right = i; break;
        }
    }

    if (left)
        *flip = FLIP_X_FALSE;
    else if (right)
        *flip = FLIP_X_TRUE;

    if (b.sensor & (LEFT | RIGHT))
        animation = &hold;
    else if (!(b.sensor & BOT))
        animation = &jumping;
    else if (left || right)
        animation = &walking;
    else
        animation = &standing;

    sprite = update (animation);

    push (&glows, { b.pos - camera, b.size, { 0.0f, 0.5f, .5f, 0.5f } });
    push (&sprites, { b.pos - camera, b.size, sprite, b.angle, *flip, 1 });
    push (&lights, { b.pos, b.size, true });

    float angle             = sin (SDL_GetTicks () / 250.f) * 25.f;
    float sword_swing_alpha = 1;

    if (b.player->sword_delay.state & START)
    {
        angle = 20.f;

        float wind_angle = 90;
        sword_swing_alpha
            = (SDL_GetTicks () - b.player->sword_delay.current) / 100.f;

        angle -= (SDL_GetTicks () - b.player->sword_delay.current);

        if (*flip & FLIP_X_FALSE)
            wind_angle = -90;

        push (&sprites,
              { b.player->sword->pos - camera, b.player->sword->size,
                update (&sword), wind_angle, *flip, sword_swing_alpha });
    }
    else
    {
        sword.current = 0;

        push (&sprites, { b.player->sword->pos - camera,
                          { 8, 16 },
                          { 0, 80, 16, 16 },
                          angle,
                          *flip,
                          1 });
    }
}

inline void add_mouse (vec2 pos)
{
    Body b = get_body ();

    b.type = MOUSE;

    b.pos  = pos;
    b.size = { 16.f, 16.f };

    push (&lights, { b.pos, b.size, false });
    push (&bodies, b);
}

inline void draw_mouse (Body *b)
{
}

#endif
