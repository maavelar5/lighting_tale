#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "collision.hpp"
#include "entities.hpp"
#include "math_data.hpp"
#include "shader.hpp"

#include "texture.hpp"
#include "time_data.hpp"
#include "utils.hpp"

#include <dirent.h>

#include "editor.hpp"
#include "playing_screen.hpp"

#include "font.xpm"
#include "spritesheet.xpm"

#include <iostream>

#include "globals.hpp"

#include "level.hpp"

#include "base64.hpp"

enum SCREENS
{
    PLAYING_SCREEN
};

SCREENS screen = PLAYING_SCREEN;

char* file_read (const char* filename)
{
    SDL_RWops* rw = SDL_RWFromFile (filename, "rb");
    if (rw == NULL)
        return NULL;

    Sint64 res_size = SDL_RWsize (rw);
    char*  res      = (char*)malloc (res_size + 1);

    Sint64 nb_read_total = 0, nb_read = 1;
    char*  buf = res;
    while (nb_read_total < res_size && nb_read != 0)
    {
        nb_read = SDL_RWread (rw, buf, 1, (res_size - nb_read_total));
        nb_read_total += nb_read;
        buf += nb_read;
    }
    SDL_RWclose (rw);
    if (nb_read_total != res_size)
    {
        free (res);
        return NULL;
    }

    res[nb_read_total] = '\0';

    return res;
}

int main (int argc, char** argv)
{
    init_levels ();

    if (levels.length == 0)
        add_level ();

    SDL_Init (SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow (
        "fair_challenge", 0, 0, W * 3, H * 2.5,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_GL_CreateContext (window);
    SDL_GL_SetSwapInterval (0);

    glewInit ();
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load support for the OGG and MOD sample/music formats
    int flags   = MIX_INIT_MP3;
    int initted = Mix_Init (flags);

    if ((initted & flags) != flags)
    {
        printf ("Mix_Init: Failed to init required ogg and mod support!\n");
        printf ("Mix_Init: %s\n", Mix_GetError ());
        // handle error
    }

    if (Mix_OpenAudio (44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        printf ("Mix_OpenAudio: %s\n", Mix_GetError ());
        exit (2);
    }

    Shader shader       = init_default_shader ();
    Shader light_shader = init_lighting_shader ();

    Texture spritesheet  = load_xpm (spritesheet_xpm);
    Texture font_texture = load_xpm (font_xpm);

    update_entities ();

    init_frame_buffer (window);

    time_data::init ();

    SDL_SetRelativeMouseMode (SDL_TRUE);

    while (run)
    {
        time_data::update ();

        switch (screen)
        {
            case PLAYING_SCREEN: playing_screen (window, shader); break;
        }

        batch_render (window, light_shader, shader, spritesheet, font_texture);

        for (int i = 0; i < bodies.length;)
        {
            if (bodies[i].state == REMOVE)
                bodies.remove (i);
            else
                i++;
        }
    }

    return 0;
}
