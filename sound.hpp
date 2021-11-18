#ifndef SOUND
#define SOUND

#include "globals.hpp"
#include "timer.hpp"
#include "utils.hpp"

#include "base64.hpp"

struct Sound
{
    Timer      timer;
    Mix_Chunk *sample;
};

inline Mix_Chunk *load_wav_file (const char *filename)
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

inline Mix_Chunk *load_wav (const char *raw_data)
{
    array<BYTE> decoded = base64_decode (raw_data);

    SDL_RWops *rw
        = SDL_RWFromMem (decoded.data, decoded.length * sizeof (BYTE));

    Mix_Chunk *sample = Mix_LoadWAV_RW (rw, 0);

    if (!sample)
    {
        printf ("Mix_LoadWAV: %s\n", Mix_GetError ());

        return load_wav_file (raw_data);
    }

    free (decoded.data);

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

    update (sound->timer);

    if (sound->timer.state & START)
        return;

    Mix_PlayChannel (-1, sound->sample, 0);
}

#endif
