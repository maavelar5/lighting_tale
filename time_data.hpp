#ifndef TIME_DATA
#define TIME_DATA

#include "globals.hpp"

namespace time_data
{
    uint frames;

    float fps, step, acumulator, frame, current, previous, interpolation;

    inline void init ()
    {
        frames     = 1;
        step       = 0.001;
        acumulator = frame = current = 0;

        previous      = (SDL_GetTicks () / 1000.f);
        interpolation = 0;
    }

    inline void update ()
    {
        current  = (SDL_GetTicks () / 1000.f);
        frame    = current - previous;
        previous = current;

        if (frame > 0.25)
        {
            frame = 0.25;
        }

        acumulator += frame;

        fps = frames / (SDL_GetTicks () / 1000.f);
        frames++;
    }
};

#endif
