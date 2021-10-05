#ifndef COLLISION
#define COLLISION

#include "globals.hpp"

struct aabb
{
    float x, w, y, h;

    static const int offset_x = 4;
    static const int offset_y = 6;
};

bool left (aabb a, aabb b)
{
    float dw = a.x + (((a.w - a.x) / 10) * aabb::offset_x);

    return (b.x >= a.x && b.w <= dw && b.y >= a.y && b.h <= a.h) ? true : false;
}

bool right (aabb a, aabb b)
{
    float dx = a.x + (((a.w - a.x) / 10) * aabb::offset_y);

    return (b.x >= dx && b.w <= a.w && b.y >= a.y && b.h <= a.h) ? true : false;
}

bool top (aabb a, aabb b)
{
    float dh = a.y + (((a.h - a.y) / 10) * aabb::offset_x);

    return (b.x >= a.x && b.w <= a.w && b.y >= a.y && b.h <= dh) ? true : false;
}

bool bot (aabb a, aabb b)
{
    float dy = a.y + (((a.h - a.y) / 10) * aabb::offset_y);

    return (b.x >= a.x && b.w <= a.w && b.y >= dy && b.h <= a.h) ? true : false;
}

void collision_spacing (Body *first, aabb a, aabb c)
{
    if (top (a, c) && first->vel.y < 0)
    {
        first->pos.y += ((c.h - c.y));
        first->sensor |= TOP;
        first->vel.y = 0;
    }
    else if (bot (a, c) && first->vel.y >= 0)
    {
        first->pos.y -= ((c.h - c.y) - 1);
        first->sensor |= BOT;
        first->vel.y = 0;
    }
    else if (left (a, c))
    {
        first->pos.x += ((c.w - c.x));
        first->sensor |= LEFT;
    }
    else if (right (a, c))
    {
        first->pos.x -= ((c.w - c.x));
        first->sensor |= RIGHT;
    }
}

aabb generate_aabb (Body *a)
{
    return {
        a->pos.x,
        a->pos.x + (a->size.x),
        a->pos.y,
        a->pos.y + (a->size.y),
    };
}

void solve (Body *first, Body *second)
{
    aabb a = generate_aabb (first);
    aabb b = generate_aabb (second);

    if (a.x < b.w && a.w > b.x && a.y < b.h && a.h > b.y)
    {
        aabb c = {
            (a.x >= b.x) ? a.x : b.x,
            (a.w <= b.w) ? a.w : b.w,
            (a.y >= b.y) ? a.y : b.y,
            (a.h <= b.h) ? a.h : b.h,
        };

        if (first->config == COLLISION_SPACING)
            collision_spacing (first, a, c);

        if (second->config == COLLISION_SPACING)
            collision_spacing (second, b, c);
    }
}

void check ()
{
    Cell *cell = first;

    while (cell)
    {
        for (int i = 0; i < cell->current - 1; i++)
        {
            for (int j = i + 1; j < cell->current; j++)
            {
                solve (cell->bodies[i], cell->bodies[j]);
            }
        }

        cell = cell->next;
    }
}

#endif
