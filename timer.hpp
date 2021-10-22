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

enum FADE_CONFIG
{
    FADE_IN  = 1,
    FADE_OUT = 2,
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

struct Fade
{
    uint  config;
    float alpha, min_alpha, max_alpha;

    Timer timer;
};

void init (Fade *fade, uint config, float min_alpha, float max_alpha,
           uint delay, uint restart_delay)
{
    uint timer_config = SIMPLE;

    if (config & LOOP)
    {
        config &= ~LOOP;
        timer_config |= LOOP;
    }

    fade->config    = config;
    fade->min_alpha = min_alpha;
    fade->max_alpha = max_alpha;

    fade->alpha = 0;

    if (restart_delay == 0)
    {
        restart_delay = delay;
    }

    if (config == (FADE_IN | FADE_OUT))
    {
        timer_config ^= TWO_WAY | SIMPLE;
    }

    fade->timer = { timer_config, delay, restart_delay, 0, 0 };
}

float update (Fade *fade)
{
    update (&fade->timer);

    float time_factor = 1.f;
    float current = fade->timer.current, ticks = SDL_GetTicks ();

    if (fade->config == (FADE_IN | FADE_OUT))
    {
        if (fade->timer.state & START)
        {
            fade->alpha
                = fade->min_alpha
                  + (((ticks - current) / (fade->timer.delay * time_factor))
                     * fade->max_alpha);
        }
        else if (fade->timer.state & WAIT)
        {
            fade->alpha = fade->max_alpha
                          - (((ticks - current)
                              / (fade->timer.restart_delay * time_factor))
                             * fade->max_alpha);
        }
    }
    else if (fade->config & FADE_IN)
    {
        if (fade->timer.state & START)
        {
            fade->alpha
                = (fade->min_alpha
                   + ((ticks - current) / (fade->timer.delay * time_factor))
                         * fade->max_alpha);
        }
        else if (fade->timer.state & DONE)
        {
            fade->alpha = fade->max_alpha;
        }
    }
    else if (fade->config & FADE_OUT)
    {
        if (fade->timer.state & START)
        {
            fade->alpha
                = fade->max_alpha
                  - (((ticks - current) / (fade->timer.delay * time_factor))
                     * fade->max_alpha);
        }
        else if (fade->timer.state & DONE)
        {
            fade->alpha = 0;
        }
    }

    return fade->alpha / 1.f;
}

#endif
