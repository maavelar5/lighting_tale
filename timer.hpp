#ifndef TIMER
#define TIMER

#include "globals.hpp"

enum TIMER_CONFIG
{
    SIMPLE  = 1,
    TWO_WAY = 2,
    LOOP    = 4,
};

enum TIMER_STATUS
{
    START        = 1,
    WAIT         = 2,
    DONE         = 4,
    JUST_WAITED  = 8,
    JUST_STARTED = 16,
    JUST_DONE    = 32,
};

struct Timer
{
    uint config, delay, restart_delay, current, state;
};

struct Animation
{
    int   current, size;
    Timer timer;

    const vec4 *sprites;
};

void set (Timer *t, uint state)
{
    t->state   = state;
    t->current = SDL_GetTicks ();
}

void update (Timer *t)
{
    uint diff = SDL_GetTicks () - t->current, just_set = NONE;

    if (t->state == NONE)
    {
        set (t, (START | JUST_STARTED));
        just_set = JUST_STARTED;
    }
    else if (t->state & DONE && t->config & LOOP)
    {
        set (t, START);
    }
    else if (t->state & START && diff >= t->delay)
    {
        if (t->config & TWO_WAY)
        {
            set (t, (WAIT | JUST_WAITED));
            just_set = JUST_WAITED;
        }
        else
        {
            set (t, (DONE | JUST_DONE));
            just_set = JUST_DONE;
        }
    }
    else if (t->state & WAIT && diff >= t->restart_delay)
    {
        set (t, DONE | JUST_DONE);
        just_set = JUST_DONE;
    }

    if (t->state & JUST_STARTED && !(just_set & JUST_STARTED))
    {
        t->state &= ~JUST_STARTED;
    }

    if (t->state & JUST_WAITED && !(just_set & JUST_WAITED))
    {
        t->state &= ~JUST_WAITED;
    }

    if (t->state & JUST_DONE && !(just_set & JUST_DONE))
    {
        t->state &= ~JUST_DONE;
    }
}

vec4 update (Animation *a)
{
    update (&a->timer);

    if (a->timer.state & DONE)
    {
        if (a->current < a->size - 1)
        {
            a->current++;
        }
        else
        {
            a->current = 0;
        }
    }

    return a->sprites[a->current];
}

#endif
