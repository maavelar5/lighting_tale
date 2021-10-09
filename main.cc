#include <SDL2/SDL.h>

#include "collision.hpp"
#include "entities.hpp"
#include "math_data.hpp"
#include "player.hpp"
#include "shader.hpp"
#include "spritesheet.xpm"
#include "texture.hpp"
#include "time_data.hpp"
#include "utils.hpp"

#include <dirent.h>

#include "editor.hpp"

void move (Body &b)
{
    switch (b.type)
    {
        case PLAYER: move_player (b); break;

        case ENEMY:
            b.vel.y += (gravity * time_data::step);
            b.pos += (b.vel * time_data::step);
            break;

        case PLATFORM: push (grid, &b); break;
        default: break;
    }
}

void update (Body &b)
{
    switch (b.type)
    {
        case PLAYER: break;
        case ENEMY: break;
        case PLATFORM: break;
        default: break;
    }
}

void draw_collision_boxes (Body b)
{
    aabb a = generate_aabb (&b);

    vec2 pos  = { a.x - camera.x, a.y - camera.y };
    vec2 size = { a.w - a.x, a.h - a.y };

    if (b.sensor & LEFT)
    {
        push (squares, { pos, { 1, size.y }, { 1, 0, 0, 1 }, 0, 0 });
    }

    if (b.sensor & RIGHT)
    {
        push (squares, {
                           pos + (vec2) { size.x, 0 },
                           { 1, size.y },
                           { 1, 0, 0, 1 },
                           0,
                           0,
                       });
    }

    if (b.sensor & TOP)
    {
        push (squares, { pos, { size.x, 1 }, { 1, 0, 0, 1 }, 0, 0 });
    }

    if (b.sensor & BOT)
    {
        push (squares, { pos + (vec2) { 0, size.y },
                         { size.x, 1 },
                         { 1, 0, 0, 1 },
                         0,
                         0 });
    }

    push (squares, { pos, size, { 1, 1, 1, 1 }, 0, 1 });
}

void draw (Body &b)
{
    if (collision_boxes)
        draw_collision_boxes (b);

    switch (b.type)
    {
        case FIRE: draw_fire (b); break;
        case PLAYER: draw_player (b); break;
        case PLATFORM: draw_platform (b); break;
        default: break;
    }
}

const vec4 sprite_coords[] = {
    { 0, 0, 16, 16 },
    { 0, 32, 16, 16 },
    { 0, 64, 16, 16 },
};

const int sprite_coords_size = sizeof (sprite_coords) / sizeof (vec4);

void draw_grid (vec2 mouse, int types)
{
    for (int i = 0; i < W / 16.f; i++)
    {
        for (int j = 0; j < H / 16.f; j++)
        {
            push (squares, {
                               { i * 16.f, j * 16.f },
                               { 16.f, 16.f },
                               { .3f, .3f, .3f, 1.f },
                               0,
                               true,
                           });
        }
    }

    vec4 sprite;

    switch (types)
    {
        case FIRE: sprite = { 0, 32, 16, 16 }; break;
        case PLAYER: sprite = { 0, 0, 16, 16 }; break;
        case PLATFORM: sprite = { 0, 64, 16, 16 }; break;
        default: sprite = { 0, 0, 0, 0 }; break;
    }

    push (sprites, { mouse, { 16, 16 }, sprite, 0, true });
    push (squares, { mouse, { 16, 16 }, { 1, 1, 0, 1 }, 0, true });
}

#include "level.hpp"

void show_collision_grid ()
{
    for (auto i = grid.first; i != 0; i = i->next)
    {
        push (squares, {
                           { (i->data.x * 100.f) - camera.x,
                             (i->data.y * 100.f) - camera.y },
                           { 100.f, 100.f },
                           { 1, 0, .5, 1 },
                           0,
                           true,
                       });
    }
}

int main (int argc, char **argv)
{
    init_levels ();

    if (levels.first == 0)
    {
        add_level ();
    }

    SDL_Init (SDL_INIT_EVERYTHING);

    time_data::init ();

    bool        run = true;
    SDL_Event   event;
    SDL_Window *window = SDL_CreateWindow ("fair_challenge", 0, 0, W * 3, H * 3,
                                           SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
                                               | SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GL_CreateContext (window);

    SDL_GL_SetSwapInterval (0);

    glewInit ();
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader       = init_default_shader ();
    Shader light_shader = init_lighting_shader ();

    Texture spritesheet = load_xpm (spritesheet_xpm);

    // add_entity ({ 0, 0 }, { 320, 8 }, PLATFORM);
    // add_entity ({ 0, 312 }, { 1280, 8 }, PLATFORM);

    // add_entity ({ 0, 0 }, { 8, 320 }, PLATFORM);
    // add_entity ({ 312, 0 }, { 8, 295 }, PLATFORM);
    // add_entity ({ 144, 144 }, { 16, 16 }, PLATFORM);
    // add_entity ({ 240, 240 }, { 16, 160 }, PLATFORM);
    // add_entity ({ 200, 210 }, { 100, 64 }, PLATFORM);

    update_entities ();

    init_player ();

    vec2 mouse        = { 0, 0 };
    int  sprite_index = FIRE;

    init_frame_buffer (window);

    add_entity ({ 0, 0 }, { 16, 16 }, FIRE);
    add_entity ({ 304, 0 }, { 16, 16 }, FIRE);
    add_entity ({ 0, 304 }, { 16, 16 }, FIRE);
    add_entity ({ 304, 304 }, { 16, 16 }, FIRE);

    while (run)
    {
        time_data::update ();

        while (SDL_PollEvent (&event))
        {
            if (event.type == SDL_QUIT)
            {
                run = false;
            }
            else if (event.type == SDL_KEYDOWN && event.key.repeat == 0)
            {
                set (controller, event);
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                init_frame_buffer (window, true);
            }
            else if (event.type == SDL_KEYUP)
            {
                unset (controller, event);
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                mouse.x = (event.motion.x / 16) * 16;
                mouse.y = (event.motion.y / 16) * 16;

                if (mouse.x >= W - 16)
                    mouse.x = W - 16;
                if (mouse.y >= H - 16)
                    mouse.y = H - 16;
            }
            else if (event.type == SDL_MOUSEWHEEL)
            {
                for (int i = 0; i < abs (event.wheel.y); i++)
                {
                    if (event.wheel.y)
                    {
                        sprite_index++;

                        if (sprite_index == MAX_TYPE)
                            sprite_index = 0;
                    }
                    else
                    {
                        sprite_index--;

                        if (sprite_index == FIRE)
                            sprite_index = MAX_TYPE - 1;
                    }
                }
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                // add_entity (mouse + camera, { 16, 16 },
                // (BODY_TYPES)sprite_index);
                update_entities (mouse + camera, { 16, 16 },
                                 (BODY_TYPES)sprite_index);
            }
        }

        while (time_data::acumulator >= time_data::step)
        {
            for_each (bodies, move);

            check ();
            reset (grid);

            time_data::acumulator -= time_data::step;
        }

        for_each (bodies, update);
        for_each (bodies, draw);

        if (just_pressed (controller.inputs, TOGGLE_EDITOR, true))
            editor = (editor) ? false : true;

        if (just_pressed (controller.inputs, TOGGLE_ASPECT_RATIO, true))
        {
            maintain_aspect_ratio = (maintain_aspect_ratio) ? false : true;
            init_frame_buffer (window, true);
        }

        if (editor)
            draw_grid (mouse, sprite_index);

        // push (sprites, { { 0, 0 }, { 320, 320 }, { 0, 0, 160, 160 }, 0, 0
        // });

        show_collision_grid ();

        batch_render (window, light_shader, shader, spritesheet);

        update (controller);
    }

    return 0;
}
