#ifndef COLLISION
#define COLLISION

#include "body.hpp"
#include "globals.hpp"

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
        first->vel.y = 0;
        first->sensor |= TOP;
    }
    else if (bot (a, c) && first->vel.y >= 0)
    {
        first->pos.y -= ((c.h - c.y) - 1);
        first->vel.y = 0;

        first->sensor |= BOT;
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
    aabb data;

    switch (a->type)
    {
        case PLAYER:
            data = {
                a->pos.x + 5.f,
                a->pos.x + (a->size.x - 4.f),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;

        case ENEMY:
            data = {
                a->pos.x + 5.f,
                a->pos.x + (a->size.x - 4.f),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;

        case PLATFORM:
            data = {
                a->pos.x,
                a->pos.x + a->size.x,
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;
        default: data = { 0, 0, 0, 0 }; break;
    }

    return data;
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

        switch (first->type)
        {
            case ENEMY: break;
            case PLAYER: collision_spacing (first, a, c); break;
            case PLATFORM: collision_spacing (second, b, c); break;
            default: break;
        }
    }
}

void check ()
{
    for (Node<Cell> *cell = grid.first; cell != 0; cell = cell->next)
    {
        if (cell->data.bodies.length <= 1)
            continue;

        List<Body *> *bodies = &cell->data.bodies;

        for (auto i = bodies->first; i != bodies->last; i = i->next)
        {
            for (auto j = i->next; j != 0; j = j->next)
            {
                if (i->data->type == PLATFORM && j->data->type == PLATFORM)
                    continue;

                solve (i->data, j->data);
            }
        }
    }
}

#endif
