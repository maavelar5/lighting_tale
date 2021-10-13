#ifndef PLAYING_SCREEN_H
#define PLAYING_SCREEN_H

#include "body.hpp"
#include "collision.hpp"
#include "controller.hpp"
#include "editor.hpp"
#include "entities.hpp"
#include "globals.hpp"

enum EDITOR_ACTIONS
{
    MOVE_UP    = 1,
    MOVE_DOWN  = 2,
    MOVE_LEFT  = 4,
    MOVE_RIGHT = 8,

    INCREASE_H = 16,
    INCREASE_W = 32,

    INCREASE_SIZE_W = 64,
    INCREASE_SIZE_H = 128,

    DECREASE_SIZE_W = 256,
    DECREASE_SIZE_H = 512,

    HIDE_EDITOR = 1024,

    INCREASE_EDITOR = 2048,
    DECREASE_EDITOR = 4096,
};

enum PLAYING_SCREEN_CONFIG
{
    VISIBLE_EDITOR  = 1,
    GRID_BOXES      = 2,
    COLLISION_BOXES = 4,
};

Controller get_playing_screen_controller ()
{
    Controller controller;

    push (&controller.map, { SDLK_d, PLAYER_RIGHT, KEYDOWN });
    push (&controller.map, { SDLK_a, PLAYER_LEFT, KEYDOWN });
    push (&controller.map, { SDLK_j, PLAYER_DASH, KEYDOWN });
    push (&controller.map, { SDLK_SPACE, PLAYER_JUMP, KEYDOWN });

    push (&controller.map, { SDLK_F1, SHOW_EDITOR, KEYDOWN });
    push (&controller.map, { SDLK_F2, TOGGLE_ASPECT_RATIO, KEYDOWN });
    push (&controller.map, { SDLK_F3, TOGGLE_COLLISION_BOXES, KEYDOWN });

    return controller;
}

Controller get_editor_screen_controller ()
{
    Controller controller;

    push (&controller.map, { SDLK_d, MOVE_RIGHT, KEYDOWN });
    push (&controller.map, { SDLK_a, MOVE_LEFT, KEYDOWN });
    push (&controller.map, { SDLK_w, MOVE_UP, KEYDOWN });
    push (&controller.map, { SDLK_s, MOVE_DOWN, KEYDOWN });

    push (&controller.map, { SDLK_UP, DECREASE_SIZE_H, KEYDOWN });
    push (&controller.map, { SDLK_LEFT, DECREASE_SIZE_W, KEYDOWN });
    push (&controller.map, { SDLK_DOWN, INCREASE_SIZE_H, KEYDOWN });
    push (&controller.map, { SDLK_RIGHT, INCREASE_SIZE_W, KEYDOWN });

    push (&controller.map, { SDLK_n, INCREASE_EDITOR, KEYDOWN });
    push (&controller.map, { SDLK_m, DECREASE_EDITOR, KEYDOWN });

    push (&controller.map, { SDLK_F1, HIDE_EDITOR, KEYDOWN });
    push (&controller.map, { SDLK_F3, TOGGLE_COLLISION_BOXES, KEYDOWN });

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

void editor_screen (SDL_Window *window, Shader shader, int *config)
{
    static int sprite_index = 0;

    static vec2 size = { 16.f, 16.f }, mouse = { 0.f, 0.f };

    static Controller controller = get_editor_screen_controller ();

    static Light *mouse_light = push (&lights, { mouse, { 16, 16 }, false });

    SDL_Event e;

    while (SDL_PollEvent (&e))
    {
        if (e.type == SDL_QUIT)
            run = false;
        else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            set (&controller, e.key.keysym.sym, KEYDOWN);
        else if (e.type == SDL_KEYUP)
            unset (&controller, e.key.keysym.sym, KEYDOWN);
        else if (e.type == SDL_MOUSEMOTION)
        {
            mouse.x = e.motion.x;
            mouse.y = e.motion.y;

            mouse_light->pos = mouse;
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
            if (e.button.button == SDL_BUTTON_LEFT)
                add_entity (mouse + camera, size, (BODY_TYPES)sprite_index);
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

    float camera_speed = 200.f;

    float inc = 100.f;

    while (time_data::acumulator >= time_data::step)
    {
        if (controller.state & MOVE_LEFT)
            camera.x -= camera_speed * time_data::step;
        if (controller.state & MOVE_RIGHT)
            camera.x += camera_speed * time_data::step;

        if (controller.state & MOVE_UP)
            camera.y -= camera_speed * time_data::step;
        if (controller.state & MOVE_DOWN)
            camera.y += camera_speed * time_data::step;

        if (controller.state & INCREASE_SIZE_W)
            size.x += inc * time_data::step;
        else if (controller.state & DECREASE_SIZE_W && size.x > 2)
            size.x -= inc * time_data::step;

        if (controller.state & INCREASE_SIZE_H)
            size.y += inc * time_data::step;
        else if (controller.state & DECREASE_SIZE_H && size.y > 2)
            size.y -= inc * time_data::step;

        time_data::acumulator -= time_data::step;
    }

    if (just_pressed (controller.inputs, HIDE_EDITOR, true))
        *config &= ~VISIBLE_EDITOR;

    if (controller.state & INCREASE_EDITOR)
    {
        W++;
        H++;
    }
    else if (controller.state & DECREASE_EDITOR && W > 321)
    {
        W--;
        H--;
    }

    glUseProgram (shader.id);
    set_uniform (shader, "u_projection", ortho (W, W));
    glUseProgram (0);

    if (mouse.x >= W - 16)
        mouse.x = W - 16;
    if (mouse.y >= H - 16)
        mouse.y = H - 16;

    show_collision_grid ();
    push (&glows, { mouse, size, { 1.0f, 0.0f, 1.0f, 1.0f } });
    draw (bodies, &controller, (*config & COLLISION_BOXES) ? true : false);
    draw_grid (mouse, size, sprite_index);

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
            set (&controller, e.key.keysym.sym, KEYDOWN);
        else if (e.type == SDL_KEYUP)
            unset (&controller, e.key.keysym.sym, KEYDOWN);
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

    if (just_pressed (controller.inputs, SHOW_EDITOR, true))
        *config |= VISIBLE_EDITOR;

    if (just_pressed (controller.inputs, TOGGLE_ASPECT_RATIO, true))
    {
        maintain_aspect_ratio = (maintain_aspect_ratio) ? false : true;
        init_frame_buffer (window, true);
    }

    if (just_pressed (controller.inputs, TOGGLE_COLLISION_BOXES, true))
        *config &= ~COLLISION_BOXES;

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
