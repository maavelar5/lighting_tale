#ifndef COLLISION
#define COLLISION

#include "body.hpp"
#include "globals.hpp"

bool left (AABB a, AABB b)
{
    float dw = a.x + (((a.w - a.x) / 10) * AABB::offset_x);

    return (b.x >= a.x && b.w <= dw && b.y >= a.y && b.h <= a.h) ? true : false;
}

bool right (AABB a, AABB b)
{
    float dx = a.x + (((a.w - a.x) / 10) * AABB::offset_y);

    return (b.x >= dx && b.w <= a.w && b.y >= a.y && b.h <= a.h) ? true : false;
}

bool top (AABB a, AABB b)
{
    float dh = a.y + (((a.h - a.y) / 10) * AABB::offset_x);

    return (b.x >= a.x && b.w <= a.w && b.y >= a.y && b.h <= dh) ? true : false;
}

bool bot (AABB a, AABB b)
{
    float dy = a.y + (((a.h - a.y) / 10) * AABB::offset_y);

    return (b.x >= a.x && b.w <= a.w && b.y >= dy && b.h <= a.h) ? true : false;
}

void collision_spacing (Body *first, AABB a, AABB b, AABB c)
{
    if (top (a, c))
    {
        if (first->vel.y < 0 && !(first->sensor & BOT))
        {
            first->pos.y += ((c.h - c.y));
            first->vel.y = 0;
        }

        first->sensor |= TOP;
    }
    else if (bot (a, c))
    {
        if (first->vel.y >= 0 && !(first->sensor & TOP))
        {
            first->pos.y -= ((c.h - c.y) - 1);
            first->vel.y = 0;
        }

        first->sensor |= BOT;
    }
    else if (left (a, c))
    {
        if (!(first->sensor & RIGHT))
            first->pos.x += ((c.w - c.x));

        first->sensor |= LEFT;
    }
    else if (right (a, c))
    {
        if (!(first->sensor & LEFT))
            first->pos.x -= ((c.w - c.x));

        first->sensor |= RIGHT;
    }
    else
    {
        // first->pos = first->prev_pos;

        if (a.x > b.x)
            first->sensor |= LEFT;
        else
            first->sensor |= RIGHT;

        if (a.y > b.y)
            first->sensor |= TOP;
        else
            first->sensor |= BOT;
    }
}

AABB generate_aabb (Body *a)
{
    AABB data;

    switch (a->type)
    {
        case PLAYER:
            data = {
                a->pos.x + 4.f,
                a->pos.x + (a->size.x - 4.f),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;

        case ENEMY:
            data = {
                a->pos.x + 2,
                a->pos.x + (a->size.x - 2),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;
        case PLAYER_SWORD:
            data = {
                a->pos.x,
                a->pos.x + (a->size.x),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;
        default:
            data = {
                a->pos.x,
                a->pos.x + a->size.x,
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;
    }

    data.size = { data.w - data.x, data.h - data.y };

    return data;
}

void solve_by_type (Body *first, Body *second, AABB a, AABB b, AABB c)
{
    switch (first->type)
    {
        case ENEMY:
            collision_spacing (first, a, b, c);

            if (second->type == PLAYER_SWORD
                && first->damageable->hit_recovery.state & DONE)
            {
                first->speed = 150.f;
                first->damageable->hp -= 1;

                if (first->damageable->hp <= 0)
                    first->state = REMOVE;

                set (first->damageable->hit_recovery, START | JUST_STARTED);
            }
            break;
        case PLAYER_SWORD:
            if (second->type == ENEMY
                && second->damageable->hit_recovery.state & DONE)
            {
                if (first->player->sword_direction & BOT)
                    first->player->body->vel.y = -250;
                else if (first->player->sword_direction & LEFT)
                    first->player->body->vel.x = 100.f;
                else if (first->player->sword_direction & RIGHT)
                    first->player->body->vel.x = -100.f;
            }
            break;
        case PLAYER:
            switch (second->type)
            {
                case PLATFORM: collision_spacing (first, a, b, c); break;
                case ENEMY:
                    if (first->damageable->hit_recovery.state & DONE)
                    {
                        if (first->pos.x < second->pos.x)
                            first->vel.x = -100.f;
                        else
                            first->vel.x = 100.f;

                        first->vel.y = -200.f;

                        set (first->damageable->hit_recovery,
                             START | JUST_STARTED);
                    }
                    break;
                default: break;
            }
            break;
        default: break;
    }
}

void check ()
{
    for (int g = 0; g < grid.length; g++)
    {
        if (grid[g].bodies.length <= 1)
            continue;

        for (int i = 0; i < grid[g].bodies.length - 1; i++)
        {
            Body *first = grid[g].bodies[i];

            for (int j = i + 1; j < grid[g].bodies.length; j++)
            {
                Body *second = grid[g].bodies[j];

                if (first->type == PLATFORM && second->type == PLATFORM)
                    continue;

                AABB a = generate_aabb (first);
                AABB b = generate_aabb (second);

                if (a.x < b.w && a.w > b.x && a.y < b.h && a.h > b.y)
                {
                    AABB c = {
                        (a.x >= b.x) ? a.x : b.x,
                        (a.w <= b.w) ? a.w : b.w,
                        (a.y >= b.y) ? a.y : b.y,
                        (a.h <= b.h) ? a.h : b.h,
                    };

                    solve_by_type (first, second, a, b, c);
                    solve_by_type (second, first, b, a, c);
                }
            }
        }
    }
}

#endif
