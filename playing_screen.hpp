#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include "body.hpp"
#include "collision.hpp"
#include "controller.hpp"
#include "editor.hpp"
#include "entities.hpp"
#include "globals.hpp"

enum PLAYING_SCREEN_CONFIG
{
    VISIBLE_EDITOR  = 1,
    GRID_BOXES      = 2,
    COLLISION_BOXES = 4,
    ASPECT_RATIO    = 8,
    LIGHT_ROOM      = 16,
};

void move (Bodies *bodies, Inputs *inputs)
{
    for (Body *n = bodies->first; n != limit (*bodies); n = n->next)
    {
        switch (n->type)
        {
            case ENEMY: move_enemy (n); break;
            case PLAYER: move_player (n, inputs); break;
            case PLATFORM: push (&grid, n); break;
            default: break;
        }
    }
}

void draw_collision_boxes (Body b)
{
    AABB a = generate_aabb (&b);

    vec2 pos  = { a.x - camera.x, a.y - camera.y };
    vec2 size = { a.w - a.x, a.h - a.y };

    if (b.sensor & LEFT)
    {
        push (&squares, { pos, { 1, size.y }, { 1, 0, 0, 1 }, 0, 0 });
    }

    if (b.sensor & RIGHT)
    {
        push (&squares, {
                            pos + (vec2) { size.x, 0 },
                            { 1, size.y },
                            { 1, 0, 0, 1 },
                            0,
                            0,
                        });
    }

    if (b.sensor & TOP)
    {
        push (&squares, { pos, { size.x, 1 }, { 1, 0, 0, 1 }, 0, 0 });
    }

    if (b.sensor & BOT)
    {
        push (&squares, { pos + (vec2) { 0, size.y },
                          { size.x, 1 },
                          { 1, 0, 0, 1 },
                          0,
                          0 });
    }

    push (&squares, { pos, size, { 1, 1, 1, 1 }, 0, 1 });
}

void draw (Bodies bodies, Inputs *inputs, bool collision_boxes)
{
    for (Body *n = bodies.first; n != limit (bodies); n = n->next)
    {
        if (collision_boxes)
            draw_collision_boxes (*n);

        switch (n->type)
        {
            case FIRE: draw_fire (*n); break;
            case ENEMY: draw_enemy (*n); break;
            case PLAYER: draw_player (*n, inputs); break;
            case PLATFORM: draw_platform (*n); break;
            default: break;
        }
    }
}

const int sprite_coords_size = MAX_BODY_TYPE;

void draw_grid (vec2 pos, vec2 size, int types)
{
    // float cell_size = 16.f;

    // for (int i = 0; i < W / cell_size; i++)
    // {
    //     for (int j = 0; j < H / cell_size; j++)
    //     {
    //         push (&squares, {
    //                             { i * cell_size, j * cell_size },
    //                             { cell_size, cell_size },
    //                             { .3f, .3f, .3f, .2f },
    //                             0,
    //                             true,
    //                         });
    //     }
    // }

    vec4 sprite;

    switch (types)
    {
        case FIRE: sprite = { 0, 32, 16, 16 }; break;
        case ENEMY: sprite = { 0, 16, 16, 16 }; break;
        case PLAYER: sprite = { 0, 0, 16, 16 }; break;
        case PLATFORM: sprite = { 0, 64, 16, 16 }; break;
        case PLAYER_SWORD: sprite = { 0, 80, 16, 16 }; break;
        default: sprite = { 0, 0, 16, 16 }; break;
    }

    push (&sprites, { pos, size, sprite, 0, 0, 1 });
    push (&squares, { pos, size, { 1, 1, 0, 1 }, 0, true });
}

void show_collision_grid ()
{
    for (auto i = grid.first; i != 0; i = i->next)
    {
        push (&squares, {
                            { (i->x * GRID_SIZE) - camera.x,
                              (i->y * GRID_SIZE) - camera.y },
                            { GRID_SIZE, GRID_SIZE },
                            { 1, 0, .5, 1 },
                            0,
                            true,
                        });
    }
}

void editor_screen (SDL_Window *window, Shader shader, int *config)
{
    static int sprite_index = 0;

    static bool clicked = false;
    static vec2 size = { 16.f, 16.f, }, mouse = { 0.f, 0.f, },
      prev    = { 0.f, 0.f, };

    static Light *light = push (&lights, { mouse, { 16, 16 }, false });

    static Inputs inputs = { 0, 0, 0, 0, 0 };

    SDL_Event e;

    uint action = 0;

    while (SDL_PollEvent (&e))
    {
        if (e.type == SDL_QUIT)
            run = false;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_w: action = MOVE_UP; break;
                case SDLK_a: action = MOVE_LEFT; break;
                case SDLK_s: action = MOVE_DOWN; break;
                case SDLK_d: action = MOVE_RIGHT; break;
                case SDLK_1: action = HIDE_EDITOR; break;
                case SDLK_n: action = DECREASE_EDITOR; break;
                case SDLK_m: action = INCREASE_EDITOR; break;
                case SDLK_UP: action = DECREASE_H; break;
                case SDLK_LEFT: action = DECREASE_W; break;
                case SDLK_DOWN: action = INCREASE_H; break;
                case SDLK_RIGHT: action = INCREASE_W; break;
            }
        }

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            push (&inputs, action, (SIMPLE | LOOP));
        else if (e.type == SDL_KEYUP)
            set_inactive (&inputs, action);
        else if (e.type == SDL_MOUSEMOTION)
        {
            if (!clicked)
            {
                prev.x = e.motion.x;
                prev.y = e.motion.y;
            }

            mouse.x = e.motion.x;
            mouse.y = e.motion.y;

            light->pos = mouse;

            size = mouse - prev;
        }
        else if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            prev.x = e.motion.x;
            prev.y = e.motion.y;

            clicked = true;
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
            clicked = false;

            if (size.x <= 8)
                size.x = 8;
            if (size.y <= 8)
                size.y = 8;

            if (e.button.button == SDL_BUTTON_LEFT)
                add_entity (prev + camera, size, (BODY_TYPES)sprite_index);
            else if (e.button.button == SDL_BUTTON_RIGHT)
                remove_entity (mouse + (size / 2));
        }
        else if (e.type == SDL_MOUSEWHEEL)
        {
            sprite_index = (e.wheel.y) ? sprite_index + 1 : sprite_index - 1;

            if (sprite_index >= sprite_coords_size)
                sprite_index = 0;

            else if (sprite_index < 0)
                sprite_index = sprite_coords_size;
        }
    }

    float inc = 100.f, camera_speed = 200.f;

    Input *move_up = 0, *move_down = 0, *move_left = 0, *move_right = 0,
          *hide_editor = 0, *increase_size_w = 0, *increase_size_h = 0,
          *decrease_size_w = 0, *decrease_size_h = 0, *increase_editor = 0,
          *decrease_editor = 0;

    for (Input *i = inputs.first; i != limit (inputs); i = i->next)
    {
        switch (i->action)
        {
            case MOVE_UP: move_up = i; break;
            case MOVE_DOWN: move_down = i; break;
            case MOVE_LEFT: move_left = i; break;
            case MOVE_RIGHT: move_right = i; break;
            case INCREASE_W: increase_size_w = i; break;
            case INCREASE_H: increase_size_h = i; break;
            case DECREASE_W: decrease_size_w = i; break;
            case DECREASE_H: decrease_size_h = i; break;
            case HIDE_EDITOR: hide_editor = i; break;
            case INCREASE_EDITOR: increase_editor = i; break;
            case DECREASE_EDITOR: decrease_editor = i; break;
        }
    }

    while (time_data::acumulator >= time_data::step)
    {
        if (move_left)
            camera.x -= camera_speed * time_data::step;
        if (move_right)
            camera.x += camera_speed * time_data::step;

        if (move_up)
            camera.y -= camera_speed * time_data::step;
        if (move_down)
            camera.y += camera_speed * time_data::step;

        if (increase_size_w)
            size.x += inc * time_data::step;
        if (decrease_size_w && size.x > 2)
            size.x -= inc * time_data::step;

        if (increase_size_h)
            size.y += inc * time_data::step;
        if (decrease_size_h && size.y > 2)
            size.y -= inc * time_data::step;

        if (increase_editor)
        {
            W += (inc * 2 * time_data::step);
            H += (inc * 2 * time_data::step);
        }

        if (decrease_editor && W > 322)
        {
            W -= (inc * 2 * time_data::step);
            H -= (inc * 2 * time_data::step);
        }

        time_data::acumulator -= time_data::step;
    }

    if (increase_editor || decrease_editor)
    {
        glUseProgram (shader.id);
        set_uniform (shader, "u_projection", ortho (W, W));
        glUseProgram (0);
    }

    if (mouse.x >= W - 16)
        mouse.x = W - 16;
    if (mouse.y >= H - 16)
        mouse.y = H - 16;

    if (size.x <= 8)
        size.x = 8;
    if (size.y <= 8)
        size.y = 8;

    show_collision_grid ();

    push (&glows, { mouse, size, { 0.5f, 0.0f, 0.5f, 0.0f } });
    draw (bodies, &inputs, (*config & COLLISION_BOXES) ? true : false);

    draw_grid (prev, size, sprite_index);

    update (&inputs);

    if (active (hide_editor))
    {
        *config ^= VISIBLE_EDITOR;
        reset (&inputs);
    }
}

void playing_screen (SDL_Window *window, Shader shader, int *config)
{
    static Inputs inputs = { 0, 0, 0, 0, 0 };

    uint      action = 0;
    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        if (e.type == SDL_QUIT)
            run = false;
        else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
        {
            switch (e.key.keysym.sym)
            {
                case SDLK_a: action = PLAYER_LEFT; break;
                case SDLK_d: action = PLAYER_RIGHT; break;
                case SDLK_1: action = SHOW_EDITOR; break;
                case SDLK_j: action = PLAYER_ATTACK; break;
                case SDLK_s: action = PLAYER_DOWN; break;
                case SDLK_w: action = PLAYER_UP; break;
                case SDLK_SPACE: action = PLAYER_JUMP; break;
            }
        }

        if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            push (&inputs, action);
        else if (e.type == SDL_KEYUP)
            set_inactive (&inputs, action);
    }

    for (Input *i = inputs.first; i != limit (inputs); i = i->next)
    {
        if (i->timer.state & (DONE | WAIT))
            continue;
        else if (i->action == SHOW_EDITOR)
        {
            *config |= VISIBLE_EDITOR;
            update_entities ();
        }
        else if (i->action == TOGGLE_COLLISION_BOXES)
            *config ^= COLLISION_BOXES;
        else if (i->action == TOGGLE_ASPECT_RATIO)
        {
            *config ^= ASPECT_RATIO;
            init_frame_buffer (window, true);
        }
    }

    while (time_data::acumulator >= time_data::step)
    {
        move (&bodies, &inputs);
        check ();
        reset (&grid);

        time_data::acumulator -= time_data::step;
    }

    // show_collision_grid ();

    draw (bodies, &inputs, (*config & COLLISION_BOXES) ? true : false);

    update (&inputs);

    if (*config & VISIBLE_EDITOR)
        reset (&inputs);
}

void playing_screen (SDL_Window *window, Shader shader)
{
    static int config = 0;

    if (config & VISIBLE_EDITOR)
        editor_screen (window, shader, &config);
    else
        playing_screen (window, shader, &config);
}

#endif
