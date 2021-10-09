#ifndef BODY
#define BODY

#include "cell.hpp"
#include "globals.hpp"
#include "time_data.hpp"
#include "timer.hpp"

enum BODY_TYPES
{
    FIRE,
    ENEMY,
    PLAYER,
    PLATFORM,
    MAX_TYPE,
};

enum BODY_CONFIG
{
    COLLISION_SPACING = 1
};

struct aabb
{
    float x, w, y, h;

    static const int offset_x = 4;
    static const int offset_y = 6;
};

struct Body
{
    uint  sensor, config;
    vec2  pos, prev, size, vel, accel;
    float speed, angle;

    BODY_TYPES type;
    Animation *animation;

    int light_index;
};

void push (List<Cell> &grid, Body *body)
{
    ivec4 locator = {
        (int)body->pos.x / 100,
        (int)body->pos.y / 100,
        (int)(body->pos.x + body->size.x) / 100,
        (int)(body->pos.y + body->size.y) / 100,
    };

    for (int i = locator.x; i <= locator.w; i++)
    {
        for (int j = locator.y; j <= locator.h; j++)
        {
            Cell &found = find (grid, { i, j });

            push (found.bodies, body);
        }
    }
}

Body get_body ()
{
    Body b;

    b.speed = b.angle = 0;
    b.sensor = b.config = NONE;
    b.prev = b.accel = b.pos = b.size = b.vel = { 0.f, 0.f };

    b.type = PLATFORM;

    b.animation = 0;

    return b;
}

List<Body> bodies;

#endif
