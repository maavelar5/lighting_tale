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

Controller get_playing_screen_controller ()
{
    Controller controller;

    push (&controller.map, { SDLK_d, PLAYER_RIGHT, SDL_KEYDOWN });
    push (&controller.map, { SDLK_a, PLAYER_LEFT, SDL_KEYDOWN });
    push (&controller.map, { SDLK_j, PLAYER_DASH, SDL_KEYDOWN });
    push (&controller.map, { SDLK_SPACE, PLAYER_JUMP, SDL_KEYDOWN });

    push (&controller.map, { SDLK_F1, SHOW_EDITOR, SDL_KEYDOWN });
    push (&controller.map, { SDLK_F2, TOGGLE_ASPECT_RATIO, SDL_KEYDOWN });
    push (&controller.map, { SDLK_F3, TOGGLE_COLLISION_BOXES, SDL_KEYDOWN });

    return controller;
}

Controller get_editor_screen_controller ()
{
    Controller controller;

    push (&controller.map, { SDLK_d, MOVE_RIGHT, SDL_KEYDOWN });
    push (&controller.map, { SDLK_a, MOVE_LEFT, SDL_KEYDOWN });
    push (&controller.map, { SDLK_w, MOVE_UP, SDL_KEYDOWN });
    push (&controller.map, { SDLK_s, MOVE_DOWN, SDL_KEYDOWN });

    push (&controller.map, { SDLK_UP, DECREASE_SIZE_H, SDL_KEYDOWN });
    push (&controller.map, { SDLK_LEFT, DECREASE_SIZE_W, SDL_KEYDOWN });
    push (&controller.map, { SDLK_DOWN, INCREASE_SIZE_H, SDL_KEYDOWN });
    push (&controller.map, { SDLK_RIGHT, INCREASE_SIZE_W, SDL_KEYDOWN });

    push (&controller.map, { SDLK_n, INCREASE_EDITOR, SDL_KEYDOWN });
    push (&controller.map, { SDLK_m, DECREASE_EDITOR, SDL_KEYDOWN });

    push (&controller.map, { SDLK_F1, HIDE_EDITOR, SDL_KEYDOWN });
    push (&controller.map, { SDLK_F3, TOGGLE_COLLISION_BOXES, SDL_KEYDOWN });

    return controller;
}

void move (Bodies *bodies, Controller *controller)
{
    for (Body *n = bodies->first; n != limit (*bodies); n = n->next)
    {
        switch (n->type)
        {
            case ENEMY: break;
            case PLAYER: move_player (n, controller); break;
            case PLATFORM: push (&grid, n); break;
            default: break;
        }
    }
}

void draw_collision_boxes (Body b)
{
    aabb a = generate_aabb (&b);

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

void draw (Bodies bodies, Controller *controller, bool collision_boxes)
{
    for (Body *n = bodies.first; n != limit (bodies); n = n->next)
    {
        if (collision_boxes)
            draw_collision_boxes (*n);

        switch (n->type)
        {
            case FIRE: draw_fire (*n); break;
            case ENEMY: break;
            case PLAYER: draw_player (*n, controller); break;
            case PLATFORM: draw_platform (*n); break;
            default: break;
        }
    }
}

const vec4 sprite_coords[] = {
    { 0, 0, 16, 16 },
    { 0, 32, 16, 16 },
    { 0, 64, 16, 16 },
};

const int sprite_coords_size = sizeof (sprite_coords) / sizeof (vec4);

void draw_grid (vec2 pos, vec2 size, int types)
{
    float cell_size = 16.f;

    for (int i = 0; i < W / cell_size; i++)
    {
        for (int j = 0; j < H / cell_size; j++)
        {
            push (&squares, {
                                { i * cell_size, j * cell_size },
                                { cell_size, cell_size },
                                { .3f, .3f, .3f, .2f },
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

    push (&sprites, { pos, size, sprite, 0, true });
    push (&squares, { pos, size, { 1, 1, 0, 1 }, 0, true });
}

void show_collision_grid ()
{
    for (auto i = grid.first; i != limit (grid); i = i->next)
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

struct Mouse
{
    vec2 pos, prev;
};

void editor_screen (SDL_Window *window, Shader shader, int *config)
{
    static int sprite_index = 0;

    static bool   clicked = false;
    static vec2   size    = { 16.f, 16.f };
    static vec2   mouse   = { 0.f, 0.f };
    static vec2   prev    = { 0.f, 0.f };
    static Light *light   = push (&lights, { mouse, { 16, 16 }, false });

    static Controller controller = get_editor_screen_controller ();

    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        if (e.type == SDL_QUIT)
            run = false;
        else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            set (&controller, e.key.keysym.sym, SDL_KEYDOWN);
        else if (e.type == SDL_KEYUP)
            unset (&controller, e.key.keysym.sym, SDL_KEYDOWN);
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
            {
                size.x = 8;
            }

            if (size.y <= 8)
            {
                size.y = 8;
            }

            if (e.button.button == SDL_BUTTON_LEFT)
                add_entity (prev + camera, size, (BODY_TYPES)sprite_index);
            else if (e.button.button == SDL_BUTTON_RIGHT)
                remove_entity (mouse + (size / 2));
        }
        else if (e.type == SDL_MOUSEWHEEL)
        {
            sprite_index = (e.wheel.y) ? sprite_index + 1 : sprite_index - 1;

            if (sprite_index > sprite_coords_size)
                sprite_index = 0;

            else if (sprite_index < 0)
                sprite_index = sprite_coords_size;
        }
    }

    float inc = 100.f, camera_speed = 200.f;

    Input *move_up         = 0;
    Input *move_down       = 0;
    Input *move_left       = 0;
    Input *move_right      = 0;
    Input *increase_size_w = 0;
    Input *increase_size_h = 0;
    Input *decrease_size_w = 0;
    Input *decrease_size_h = 0;

    FOR (Input, controller.inputs)
    {
        switch (i->action)
        {
            case MOVE_UP: move_up = i; break;
            case MOVE_DOWN: move_down = i; break;
            case MOVE_LEFT: move_left = i; break;
            case MOVE_RIGHT: move_right = i; break;
            case INCREASE_SIZE_W: increase_size_w = i; break;
            case INCREASE_SIZE_H: increase_size_h = i; break;
            case DECREASE_SIZE_W: decrease_size_w = i; break;
            case DECREASE_SIZE_H: decrease_size_h = i; break;
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

        time_data::acumulator -= time_data::step;
    }

    if (active (controller.inputs, HIDE_EDITOR))
        *config &= ~VISIBLE_EDITOR;

    if (find (controller.inputs, INCREASE_EDITOR))
    {
        W += 2;
        H += 2;
    }
    else if (find (controller.inputs, DECREASE_EDITOR) && W > 322)
    {
        W -= 2;
        H -= 2;
    }

    glUseProgram (shader.id);
    set_uniform (shader, "u_projection", ortho (W, W));
    glUseProgram (0);

    if (mouse.x >= W - 16)
        mouse.x = W - 16;
    if (mouse.y >= H - 16)
        mouse.y = H - 16;

    if (size.x <= 8)
        size.x = 8;
    if (size.y <= 8)
        size.y = 8;

    show_collision_grid ();
    push (&glows, { mouse, { 8, 8 }, { 1.0f, 0.0f, 1.0f, 1.0f } });
    draw (bodies, &controller, (*config & COLLISION_BOXES) ? true : false);

    draw_grid (prev, size, sprite_index);

    update (&controller);
}

void playing_screen (SDL_Window *window, Shader shader, int *config)
{
    static Controller controller = get_playing_screen_controller ();

    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        if (e.type == SDL_QUIT)
            run = false;
        else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            set (&controller, e.key.keysym.sym, SDL_KEYDOWN);
        else if (e.type == SDL_KEYUP)
            unset (&controller, e.key.keysym.sym, SDL_KEYDOWN);
    }

    while (time_data::acumulator >= time_data::step)
    {
        move (&bodies, &controller);
        check ();
        reset (&grid);

        time_data::acumulator -= time_data::step;
    }

    show_collision_grid ();

    draw (bodies, &controller, (*config & COLLISION_BOXES) ? true : false);

    FOR (Input, controller.inputs)
    {
        if (i->timer.state & (DONE | WAIT))
            continue;
        else if (i->action == SHOW_EDITOR)
            *config |= VISIBLE_EDITOR;
        else if (i->action == TOGGLE_ASPECT_RATIO)
        {
            if (*config & ASPECT_RATIO)
                *config &= ~ASPECT_RATIO;
            else
                *config |= ASPECT_RATIO;

            init_frame_buffer (window, true);
        }
        else if (i->action == TOGGLE_COLLISION_BOXES)
        {
            *config &= ~COLLISION_BOXES;
        }
    }

    update (&controller);
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
