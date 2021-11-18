#ifndef BODY
#define BODY

#include "cell.hpp"
#include "controller.hpp"
#include "globals.hpp"
#include "shader.hpp"
#include "time_data.hpp"
#include "timer.hpp"

enum BODY_TYPES
{
    FIRE,
    ENEMY,
    PLAYER,
    PLATFORM,
    PLAYER_SWORD,

    MAX_BODY_TYPE,
};

enum BODY_STATE
{
    IDLE   = 1,
    ACTIVE = 2,
    REMOVE = 3,
};

struct AABB
{
    float x, w, y, h;

    vec2 size;

    static const int offset_x = 2;
    static const int offset_y = 8;
};

struct Damageable
{
    int   hp, current_hp;
    Timer hit_recovery;
};

struct Player
{
    int   power;
    uint  flip, sword_direction;
    Body *sword, *body;
    Fade  sword_fade;
    Timer sword_delay;
};

struct Body
{
    uint  sensor, state;
    vec2  pos, prev_pos, size, vel, accel;
    AABB  aabb;
    float speed, max_speed, angle;

    Player *    player;
    Animation * animation;
    Damageable *damageable;

    BODY_TYPES type;
};

const int GRID_SIZE = 100;

inline void push (Grid &grid, Body *body)
{
    ivec4 locator = {
        (int)body->pos.x / GRID_SIZE,
        (int)body->pos.y / GRID_SIZE,
        (int)(body->pos.x + body->size.x) / GRID_SIZE,
        (int)(body->pos.y + body->size.y) / GRID_SIZE,
    };

    for (int i = locator.x; i <= locator.w; i++)
        for (int j = locator.y; j <= locator.h; j++)
            find (grid, { i, j }).bodies.push (body);
}

inline Body get_body ()
{
    Body b;

    b.state    = ACTIVE;
    b.sensor   = NONE;
    b.prev_pos = b.accel = b.pos = b.size = b.vel = { 0.f, 0.f };
    b.max_speed = b.speed = b.angle = 0;

    b.type = PLATFORM;

    b.player     = 0;
    b.animation  = 0;
    b.damageable = 0;

    return b;
}

typedef array<Body> Bodies;

static Bodies bodies (1000, { 0 });

#endif
