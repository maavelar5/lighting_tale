#include <SDL2/SDL.h>

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

enum SCREENS
{
    PLAYING_SCREEN
};

SCREENS screen = PLAYING_SCREEN;

#include "level.hpp"

int main (int argc, char **argv)
{
    init_levels ();

    if (levels.first == 0)
        add_level ();

    SDL_Init (SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow (
        "fair_challenge", 0, 0, W * 3, H * 2.5,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

    SDL_GL_CreateContext (window);
    SDL_GL_SetSwapInterval (0);

    glewInit ();
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader  shader       = init_default_shader ();
    Shader  light_shader = init_lighting_shader ();
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
    }

    return 0;
}
