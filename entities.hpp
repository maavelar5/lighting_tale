#ifndef ENTITIES
#define ENTITIES

#include "body.hpp"
#include "collision.hpp"
#include "shader.hpp"
#include "sound.hpp"

#include "embedded_data.hpp"

inline void add_platform (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = size;
    b.type = PLATFORM;

    b.aabb = generate_aabb (&b);

    bodies.push (b);
}

inline void add_enemy (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos   = pos;
    b.size  = size;
    b.type  = ENEMY;
    b.speed = 50.f;

    b.aabb = generate_aabb (&b);

    b.vel = { 0.f, 0.f };

    Body &enemy = bodies.push (b);

    enemy.damageable = new Damageable ();

    *enemy.damageable = { 2, 2, { SIMPLE, 400, 0, 0, DONE } };
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

    bodies.push (b);
}

inline void draw_platform (Body &b)
{
    Sprite sprite;

    sprite.angle  = 0;
    sprite.sprite = { 0, 64, 16, 16 };
    sprite.flip   = FLIP_X_TRUE;

    sprite.pos  = b.pos - camera;
    sprite.size = b.size;

    squares.push ({ sprite.pos, sprite.size, { 0, .5, .5, 0.2 }, 0, 0 });
}

inline void draw_fire (Body &b)
{
    glows.push ({ b.pos - camera, b.size, { 0.5f, 0.3f, 0.0f, 0.3f } });
    lights.push ({ b.pos, b.size, true });
    sprites.push ({
        b.pos - camera,
        b.size,
        update (*b.animation),
        0,
        FLIP_X_FALSE,
        1,
    });
}

inline void draw_enemy (Body &b)
{
    // push (&glows, { b.pos - camera, b.size, { 1.f, 1.f, 0.0f } });

    sprites.push ({ b.pos - camera,
                    b.size,
                    { 0.f, 16.f, 16.f, 16.f },
                    SDL_GetTicks () / 3.f,
                    FLIP_X_FALSE,
                    1 });
}

inline void add_player (vec2 pos, vec2 size)
{
    Body body = get_body ();

    body.pos         = pos;
    body.size        = size;
    body.type        = PLAYER;
    body.speed       = 300.f;
    body.accel       = { 0.f, 0.f };
    body.max_speed   = 200.f;
    body.damageable  = new Damageable ();
    *body.damageable = { 1, 1, { TWO_WAY, 300, 1000, 0, DONE } };

    Body &b = bodies.push (body);

    b.player = new Player ();

    b.player->body        = &b;
    b.player->flip        = NONE;
    b.player->sword_delay = { TWO_WAY, 250, 250, 0, DONE };

    body.pos  = b.pos;
    body.type = PLAYER_SWORD;
    body.size = { 16.f, 16.f };

    init (b.player->sword_fade, FADE_OUT, 0, .6f, 500, 0);

    b.player->sword         = &bodies.push (body);
    b.player->sword->player = b.player;
}

inline void move_enemy (Body &b)
{
    b.prev_pos = b.pos;

    if (b.sensor)
    {
        if (b.sensor & TOP)
            b.vel.y = b.speed;
        if (b.sensor & BOT)
            b.vel.y = -b.speed;
        if (b.sensor & LEFT)
            b.vel.x = b.speed;
        if (b.sensor & RIGHT)
            b.vel.x = -b.speed;
    }

    static Sound damaged_sound = {
        { SIMPLE, 300, 50, 0, 0 },
        load_wav (damaged_raw),
    };

    b.prev_pos = b.pos;

    if (b.damageable->hit_recovery.state & JUST_STARTED)
    {
        play (&damaged_sound);
    }

    b.vel.y += ((gravity / 8) * time_data::step);
    b.pos += (b.vel * time_data::step);
    b.sensor = NONE;

    push (grid, &b);

    update (b.damageable->hit_recovery);
}

inline void move_camera (Body b)
{
    if ((b.accel.x > 0 || b.vel.x > 0) && b.pos.x - camera.x > ((W / 6) * 3))
        camera.x += fabs (b.pos.x - b.prev_pos.x);
    else if ((b.accel.x < 0 || b.vel.x < 0) && b.pos.x - camera.x < (W / 6) * 2)
        camera.x -= fabs (b.pos.x - b.prev_pos.x);

    if (b.vel.y > 0 && b.pos.y - camera.y > ((H / 6) * 3) && !(b.sensor & BOT))
        camera.y += fabs (b.pos.y - b.prev_pos.y);
    else if (b.vel.y < 0 && b.pos.y - camera.y < ((H / 6) * 2)
             && !(b.sensor & TOP))
        camera.y -= fabs (b.pos.y - b.prev_pos.y);

    if (camera.y < 0)
        camera.y = 0;
    if (camera.x < 0)
        camera.x = 0;
}

inline void move_player (Body &b, Inputs &inputs)
{
    static Sound attack_sound = {
        { SIMPLE, 300, 50, 0, 0 },
        load_wav (attack_raw),
    };

    static Sound jump_sound = {
        { SIMPLE, 300, 50, 0, 0 },
        load_wav (jump_raw),
    };

    static Sound damaged_sound = {
        { SIMPLE, 300, 50, 0, 0 },
        load_wav (damaged_raw),
    };

    b.prev_pos = b.pos;

    if (b.damageable->hit_recovery.state & JUST_STARTED)
    {
        play (&damaged_sound);
    }

    update (b.damageable->hit_recovery);

    if (b.damageable->hit_recovery.state & START)
    {
        b.sensor = NONE;
        b.vel.y += (gravity * time_data::step);
        b.pos += (b.vel * time_data::step);
        b.player->sword->pos = b.pos;

        move_camera (b);

        push (grid, &b);
        return;
    }

    Input *left = 0, *jump = 0, *right = 0, *attack = 0, *down = 0, *up = 0;

    Body *  sword  = b.player->sword;
    Player *player = b.player;

    for (int i = 0; i < inputs.length; i++)
    {
        Input &input = inputs[i];

        switch (input.action)
        {
            case PLAYER_UP: up = &input; break;
            case PLAYER_JUMP: jump = &input; break;
            case PLAYER_LEFT: left = &input; break;
            case PLAYER_RIGHT: right = &input; break;
            case PLAYER_DOWN: down = &input; break;
            case PLAYER_ATTACK: attack = &input; break;
        }
    }

    if (active (jump))
    {
        if (b.sensor & (LEFT | RIGHT) && !(b.sensor & BOT))
        {
            play (&jump_sound);

            set (jump->timer, DONE);

            b.vel.x = (b.vel.x > 0) ? -b.max_speed : b.max_speed;
            b.vel.y = -250.f;
        }
        else if (b.sensor & BOT)
        {
            play (&jump_sound);

            set (jump->timer, DONE);
            b.vel.y = -250.f;
        }
    }

    if (left || right)
    {
        if (left)
        {
            b.accel.x = (b.vel.x > 0) ? -b.speed * 8 : -b.speed;

            if (b.vel.x < -b.max_speed)
                b.accel.x = 0;
        }
        else if (right)
        {
            b.accel.x = (b.vel.x < 0) ? b.speed * 8 : b.speed;

            if (b.vel.x > b.max_speed)
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

    if (!(b.sensor & BOT))
    {
        float local_gravity = gravity;
        float max_gravity   = 400.f;

        if (b.vel.y > 0)
        {
            if (b.sensor & LEFT && left)
                max_gravity /= 8.f;
            else if (b.sensor & RIGHT && right)
                max_gravity /= 8.f;
        }

        b.vel.y += (local_gravity * time_data::step);

        if (b.vel.y > max_gravity)
            b.vel.y = max_gravity;
    }

    b.pos += (b.vel * time_data::step);

    move_camera (b);

    b.sensor = NONE;

    sword->pos = b.pos;

    if (active (attack) && b.player->sword_delay.state & DONE)
        set (b.player->sword_delay, START | JUST_STARTED);

    if (b.player->sword_delay.state & JUST_STARTED)
    {
        play (&attack_sound);

        if (up)
            b.player->sword_direction = TOP;
        else if (down)
            b.player->sword_direction |= BOT;
        else if (b.player->flip == FLIP_X_FALSE)
            b.player->sword_direction |= LEFT;
        else
            b.player->sword_direction |= RIGHT;
    }

    if (b.player->sword_delay.state & START)
    {
        sword->size = { 16.f, 16.f };

        if (player->sword_direction & TOP)
            sword->pos.y -= sword->size.y / 2.f;
        else if (player->sword_direction & BOT)
            sword->pos.y += b.size.y / 2.f;

        if (player->sword_direction & LEFT)
            sword->pos.x -= sword->size.x / 2.f;
        else if (player->sword_direction & RIGHT)
            sword->pos.x += b.size.x / 2.f;

        push (grid, b.player->sword);
    }
    else
    {
        sword->size = { 8.f, 16.f };

        player->sword_direction = NONE;

        if (b.player->flip == FLIP_X_TRUE)
            sword->pos.x -= sword->size.x / 2;
        else
            sword->pos.x += b.size.x - 4;
    }

    push (grid, &b);

    update (b.player->sword_delay);
}

inline void draw_player (Body b, Inputs &inputs)
{
    static vec4 standing_coords[] = {
        { 0, 0, 16, 16 },
        { 16, 0, 16, 16 },
        { 32, 0, 16, 16 },
        { 16, 0, 16, 16 },
    };

    static Animation standing = {
        0,
        4,
        { SIMPLE | LOOP, 150, 0, 0, 0 },
        standing_coords,
    };

    static vec4 walking_coords[] = {
        { 48, 0, 16, 16 },
        { 64, 0, 16, 16 },
        { 80, 0, 16, 16 },
    };

    static vec4 sword_coords[] = {
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
        3,
        { SIMPLE | LOOP, 75, 0, 0, 0 },
        sword_coords,
    };

    // load sample.wav in to sample
    static Sound walk_sound = {
        { SIMPLE | LOOP, 400, 50, 0, 0 },
        load_wav (walk_raw),
    };

    uint *flip = &b.player->flip;

    vec4 sprite;

    Input *left = 0, *jump = 0, *right = 0, *attack = 0, *down = 0, *up = 0;

    for (int i = 0; i < inputs.length; i++)
    {
        Input &input = inputs[i];

        switch (input.action)
        {
            case PLAYER_UP: up = &input; break;
            case PLAYER_JUMP: jump = &input; break;
            case PLAYER_LEFT: left = &input; break;
            case PLAYER_RIGHT: right = &input; break;
            case PLAYER_DOWN: down = &input; break;
            case PLAYER_ATTACK: attack = &input; break;
        }
    }

    if (left)
        *flip = FLIP_X_FALSE;
    else if (right)
        *flip = FLIP_X_TRUE;

    if (b.sensor & (LEFT | RIGHT))
        b.animation = &hold;
    else if (!(b.sensor & BOT))
        b.animation = &jumping;
    else if (left || right)
        b.animation = &walking;
    else
        b.animation = &standing;

    sprite = update (*b.animation);

    glows.push ({ b.pos - camera, b.size, { 0.01f, 0.01f, 0.01f, 1.f } });
    lights.push ({ b.pos, b.size, true });
    sprites.push ({ b.pos - camera, b.size, sprite, b.angle, *flip, 1 });

    float angle             = sin (SDL_GetTicks () / 250.f) * 25.f;
    float sword_swing_alpha = 1;

    if (b.sensor & BOT && b.vel.x)
        update (&walk_sound);

    if (b.player->sword_delay.state & START)
    {
        angle = 20.f;

        float wind_angle = 90;

        // sword_swing_alpha
        // = 1 - ((SDL_GetTicks () - b.player->sword_delay.current) /
        // 100.f);

        sword_swing_alpha = update (b.player->sword_fade);

        angle -= (SDL_GetTicks () - b.player->sword_delay.current);

        if (*flip & FLIP_X_FALSE)
            wind_angle = -90;

        if (down)
            wind_angle = 180;
        else if (up)
            wind_angle = 0;

        sprites.push ({
            b.player->sword->pos - camera,
            b.player->sword->size,
            { 16, 80, 16, 16 },
            // update (&sword),
            wind_angle,
            *flip,
            sword_swing_alpha,
        });
    }
    else
    {
        set (sword.timer, NONE);
        set (b.player->sword_fade.timer, NONE);
        sword.current = 0;

        sprites.push ({ b.player->sword->pos - camera,
                        b.player->sword->size,
                        { 0, 80, 16, 16 },
                        angle,
                        *flip,
                        1 });
    }
}

inline void add_mouse (vec2 pos)
{
    Body b = get_body ();

    b.pos  = pos;
    b.size = { 16.f, 16.f };

    lights.push ({ b.pos, b.size, false });
    bodies.push (b);
}

inline void draw_mouse (Body *b)
{
}

#endif
