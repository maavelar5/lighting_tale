#ifndef SOUND
#define SOUND

#include "globals.hpp"
#include "timer.hpp"

struct Sound
{
    Timer      timer;
    Mix_Chunk *sample;
};

inline Mix_Chunk *load_wav (const char *filename)
{
    Mix_Chunk *sample = Mix_LoadWAV (filename);

    if (!sample)
    {
        printf ("Mix_LoadWAV: %s\n", Mix_GetError ());
        // handle error
        return 0;
    }

    if (Mix_PlayChannel (-1, sample, 0) == -1)
    {
        printf ("Mix_PlayChannel: %s\n", Mix_GetError ());
        // may be critical error, or maybe just no channels were free.
        // you could allocated another channel in that case...
        return 0;
    }

    return sample;
}

inline void play (Sound *sound)
{
    if (sound->sample)
        Mix_PlayChannel (-1, sound->sample, 0);
}

inline void update (Sound *sound)
{
    if (!sound->sample)
        return;

    update (&sound->timer);

    if (sound->timer.state & START)
        return;

    Mix_PlayChannel (-1, sound->sample, 0);
}

#endif
