#ifndef BODY
#define BODY

#include "cell.hpp"
#include "controller.hpp"
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
    COLLISION_SPACING  = 1,
    SHOW_COLLISION_BOX = 2,
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
    vec2  pos, prev_pos, size, vel, accel;
    float speed, angle;

    BODY_TYPES type;
    Animation *animation;

    NODE_PROPERTIES (Body);
};

const int GRID_SIZE = 100;

void push (Grid *grid, Body *body)
{
    ivec4 locator = {
        (int)body->pos.x / GRID_SIZE,
        (int)body->pos.y / GRID_SIZE,
        (int)(body->pos.x + body->size.x) / GRID_SIZE,
        (int)(body->pos.y + body->size.y) / GRID_SIZE,
    };

    for (int i = locator.x; i <= locator.w; i++)
    {
        for (int j = locator.y; j <= locator.h; j++)
        {
            Cell *found = find (grid, { i, j });

            push (&found->bodies, { body, 0, 0 });
        }
    }
}

Body get_body ()
{
    Body b;

    b.speed = b.angle = 0;
    b.sensor = b.config = NONE;
    b.prev_pos = b.accel = b.pos = b.size = b.vel = { 0.f, 0.f };

    b.type = PLATFORM;

    b.animation = 0;

    b.next = b.prev = 0;

    return b;
}

struct Bodies
{
    LIST_PROPERTIES (Body);
};

COMMON_FUNCTIONS (Body, Bodies)

Bodies bodies;

#endif
