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
};

Controller get_playing_screen_controller ()
{
    Controller controller;

    push (controller.map, { SDLK_d, PLAYER_RIGHT, KEYDOWN });
    push (controller.map, { SDLK_a, PLAYER_LEFT, KEYDOWN });
    push (controller.map, { SDLK_j, PLAYER_DASH, KEYDOWN });
    push (controller.map, { SDLK_SPACE, PLAYER_JUMP, KEYDOWN });

    push (controller.map, { SDLK_F1, TOGGLE_EDITOR, KEYDOWN });
    push (controller.map, { SDLK_F2, TOGGLE_ASPECT_RATIO, KEYDOWN });
    push (controller.map, { SDLK_F3, TOGGLE_COLLISION_BOXES, KEYDOWN });

    return controller;
}

Controller get_editor_screen_controller ()
{
    Controller controller;

    push (controller.map, { SDLK_d, MOVE_RIGHT, KEYDOWN });
    push (controller.map, { SDLK_a, MOVE_LEFT, KEYDOWN });
    push (controller.map, { SDLK_w, MOVE_UP, KEYDOWN });
    push (controller.map, { SDLK_s, MOVE_DOWN, KEYDOWN });

    push (controller.map, { SDLK_b, INCREASE_W, KEYDOWN });

    push (controller.map, { SDLK_F1, TOGGLE_EDITOR, KEYDOWN });
    push (controller.map, { SDLK_F3, TOGGLE_COLLISION_BOXES, KEYDOWN });

    return controller;
}

void move (List<Body> bodies, Controller *controller)
{
    for (auto n = bodies.first; n != limit (bodies); n = n->next)
    {
        Body &b = n->data;

        switch (b.type)
        {
            case ENEMY: break;
            case PLAYER: move_player (b, controller); break;
            case PLATFORM: push (grid, &b); break;
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

void draw (List<Body> bodies, Controller *controller, bool collision_boxes)
{
    for (auto n = bodies.first; n != limit (bodies); n = n->next)
    {
        Body &b = n->data;

        if (collision_boxes)
            draw_collision_boxes (b);

        switch (b.type)
        {
            case FIRE: draw_fire (b); break;
            case ENEMY: break;
            case PLAYER: draw_player (b, controller); break;
            case PLATFORM: draw_platform (b); break;
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

void show_collision_grid ()
{
    for (auto i = grid.first; i != limit (grid); i = i->next)
    {
        push (squares, {
                           { (i->data.x * GRID_SIZE) - camera.x,
                             (i->data.y * GRID_SIZE) - camera.y },
                           { GRID_SIZE, GRID_SIZE },
                           { 1, 0, .5, 1 },
                           0,
                           true,
                       });
    }
}

void playing_screen (SDL_Window *window, Shader shader)
{
    static int sprite_index = 0;

    static vec2 mouse  = { 0.f, 0.f };
    static bool editor = false, collision_boxes = false;

    static Controller playing_controller = get_playing_screen_controller ();
    static Controller editor_controller  = get_editor_screen_controller ();

    static Light *mouse_light = &push (lights, { mouse, { 16, 16 }, false });

    SDL_Event e;

    Controller *controller = &playing_controller;

    if (editor)
    {
        controller = &editor_controller;
        reset (playing_controller.inputs);
    }
    else
        reset (editor_controller.inputs);

    while (SDL_PollEvent (&e))
    {
        if (e.type == SDL_QUIT)
            run = false;
        else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
            set (*controller, e.key.keysym.sym, KEYDOWN);
        else if (e.type == SDL_KEYUP)
            unset (*controller, e.key.keysym.sym, KEYDOWN);
        else if (e.type == SDL_MOUSEMOTION)
        {
            // mouse.x = (e.motion.x / 16) * 16;
            // mouse.y = (e.motion.y / 16) * 16;

            // if (mouse.x >= W - 16)
            //     mouse.x = W - 16;
            // if (mouse.y >= H - 16)
            //     mouse.y = H - 16;

            mouse.x = e.motion.x;
            mouse.y = e.motion.y;

            mouse_light->pos = mouse;
        }
        else if (e.type == SDL_MOUSEBUTTONUP)
        {
            if (editor)
            {
                if (e.button.button == SDL_BUTTON_LEFT)
                    add_entity (mouse + camera, { 16, 16 },
                                (BODY_TYPES)sprite_index);
                else if (e.button.button == SDL_BUTTON_RIGHT)
                    remove_entity (mouse + (vec2) { 8.f, 8.f });
            }
        }
        else if (e.type == SDL_MOUSEWHEEL)
        {
            if (e.wheel.y)
            {
                sprite_index++;

                if (sprite_index > sprite_coords_size)
                    sprite_index = 0;
            }
            else
            {
                sprite_index--;

                if (sprite_index < 0)
                    sprite_index = sprite_coords_size;
            }
        }
    }

    while (time_data::acumulator >= time_data::step)
    {
        if (editor)
        {
            float camera_speed = 320.f;

            if (controller->state & MOVE_LEFT)
                camera.x -= camera_speed * time_data::step;
            if (controller->state & MOVE_RIGHT)
                camera.x += camera_speed * time_data::step;

            if (controller->state & MOVE_UP)
                camera.y -= camera_speed * time_data::step;
            if (controller->state & MOVE_DOWN)
                camera.y += camera_speed * time_data::step;
        }
        else
        {
            move (bodies, controller);
            check ();
            reset (grid);
        }

        time_data::acumulator -= time_data::step;
    }

    show_collision_grid ();

    push (glows, { mouse, { 16, 16 }, { 1.0f, 0.0f, 1.0f, 1.0f } });
    draw (bodies, controller, collision_boxes);

    if (just_pressed (controller->inputs, TOGGLE_EDITOR, true))
        editor = (editor) ? false : true;

    if (just_pressed (controller->inputs, TOGGLE_ASPECT_RATIO, true))
    {
        maintain_aspect_ratio = (maintain_aspect_ratio) ? false : true;
        init_frame_buffer (window, true);
    }

    if (just_pressed (controller->inputs, TOGGLE_COLLISION_BOXES, true))
    {
        collision_boxes = true;
    }

    if (editor)
    {
        if (controller->state & INCREASE_W)
        {
            W += 32;
            H += 32;

            if (maintain_aspect_ratio)
                printf ("%.2f -- %.2f\n", W, H);

            glUseProgram (shader.id);
            set_uniform (shader, "u_projection", ortho (W, W));
            glUseProgram (0);
        }

        draw_grid (mouse, sprite_index);
    }

    update (*controller);
}

#endif
