#include <SDL2/SDL.h>

#include "math_data.hpp"
#include "shader.hpp"
#include "spritesheet.xpm"
#include "texture.hpp"
#include "time_data.hpp"
#include "utils.hpp"

const float gravity = 800.f;

bool editor = false;

struct Cell
{
    int          x, y;
    List<Body *> bodies, platforms;

    bool operator== (Cell cell)
    {
        if (x == cell.x && y == cell.y)
            return true;
        else
            return false;
    }
};

Cell &find (List<Cell> &grid, Cell cell)
{
    for (Node<Cell> *i = grid.first; i != 0; i = i->next)
    {
        if (i->data == cell)
            return i->data;
    }

    return push (grid, cell);
}

void reset (List<Cell> &grid)
{
    for (Node<Cell> *i = grid.first; i != 0; i = i->next)
    {
        reset (i->data.bodies);
    }

    grid.current = grid.first;
    grid.length  = 0;
}

List<Body> bodies;
List<Cell> grid;

enum INPUT_TYPE
{
    CLICK,
    BUTTON,
    KEYDOWN,
};

enum ACTIONS
{
    NO_ACTION = 0,

    CARET_UP    = 1,
    CARET_DOWN  = 2,
    CARET_LEFT  = 4,
    CARET_RIGHT = 8,

    PLAYER_JUMP  = 16,
    PLAYER_LEFT  = 32,
    PLAYER_RIGHT = 64,

    TOGGLE_EDITOR = 128,
};

struct Input
{
    int        data;
    ACTIONS    action;
    INPUT_TYPE type;

    uint time;
    bool active;

    bool operator== (Input i)
    {
        if (data == i.data && action == i.action && type == i.type)
            return true;
        else
            return false;
    }
};

struct Controller
{
    uint        state;
    List<Input> map, inputs;
};

Input *find (List<Input> map, int data, INPUT_TYPE type)
{
    for (auto i = map.first; i != limit (map); i = i->next)
    {
        Input input = i->data;

        if (input.data == data && input.type == type)
        {
            return &i->data;
        }
    }

    return 0;
}

Input *find (List<Input> inputs, ACTIONS action)
{
    for (auto i = inputs.first; i != limit (inputs); i = i->next)
    {
        Input input = i->data;

        if (input.action == action)
        {
            return &i->data;
        }
    }

    return 0;
}

bool just_pressed (List<Input> inputs, ACTIONS action, bool uncheck = false)
{
    Input *input = find (inputs, action);

    if (!input)
        return false;

    if (SDL_GetTicks () - input->time <= 100)
    {
        if (uncheck)
            input->time = 0;
        return true;
    }
    else
    {
        return false;
    }
}

void remove (List<Input> &inputs, Input value)
{
    for (auto i = inputs.first; i != limit (inputs);)
    {
        Input input = i->data;

        if (input == value)
        {
            i = remove (inputs, i);
        }
        else
        {
            i = i->next;
        }
    }
}

void print (List<Input> inputs)
{
    for (auto i = inputs.first; i != limit (inputs); i = i->next)
    {
        printf ("%d => ", i->data.data);
    }

    printf ("length: %zu\n", inputs.size);
}

void set (Controller &controller, SDL_Event e)
{
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
    {
        Input *input = find (controller.map, e.key.keysym.sym, KEYDOWN);

        if (input)
        {
            input->time   = SDL_GetTicks ();
            input->active = true;

            controller.state |= input->action;
            push (controller.inputs, *input);
        }
    }
}

void unset (Controller &controller, SDL_Event e)
{
    if (e.type == SDL_KEYUP)
    {
        Input *input = find (controller.inputs, e.key.keysym.sym, KEYDOWN);

        if (input)
            input->active = false;
    }
}

void update (Controller &controller)
{
    for (auto i = controller.inputs.first; i != limit (controller.inputs);)
    {
        if (!i->data.active && SDL_GetTicks () - i->data.time > 100)
        {
            controller.state &= ~i->data.action;
            i = remove (controller.inputs, i);
        }
        else
        {
            i = i->next;
        }
    }
}

vec2 camera = { 0.f, 0.f };

Controller get_controller ()
{
    Controller controller;

    push (controller.map, { SDLK_d, PLAYER_RIGHT, KEYDOWN });
    push (controller.map, { SDLK_a, PLAYER_LEFT, KEYDOWN });
    push (controller.map, { SDLK_SPACE, PLAYER_JUMP, KEYDOWN });
    push (controller.map, { SDLK_F1, TOGGLE_EDITOR, KEYDOWN });

    return controller;
}

Controller controller = get_controller ();

enum BODY_TYPES
{
    ENEMY,
    PLAYER,
    PLATFORM,
};

enum BODY_CONFIG
{
    COLLISION_SPACING = 1
};

struct Body
{
    uint  sensor, config;
    vec2  pos, prev, size, vel, accel;
    float speed, angle;

    BODY_TYPES type;

    vec4 sprite;
};

void push (List<Cell> &grid, Body *body)
{
    ivec4 locator = {
        (int)body->pos.x / 100,
        (int)body->pos.y / 100,
        (int)(body->pos.x + body->size.x) / 100,
        (int)(body->pos.y + body->size.y) / 100,
    };

    for (int i = locator.x; i <= locator.w; i++)
    {
        for (int j = locator.y; j <= locator.h; j++)
        {
            Cell &found = find (grid, { i, j });

            push (found.bodies, body);
        }
    }
}

void move_player (Body &b)
{
    b.prev = b.pos;

    if (just_pressed (controller.inputs, PLAYER_JUMP))
    {
        if (b.sensor & (LEFT | RIGHT) && !(b.sensor & BOT))
        {
            b.vel.x = (b.vel.x > 0) ? -b.speed : b.speed;
            b.vel.y = -250.f;
        }
        else if (b.sensor & BOT)
        {
            b.vel.y = -250.f;
        }
    }

    if (controller.state & (PLAYER_LEFT | PLAYER_RIGHT))
    {
        if (controller.state & PLAYER_LEFT)
        {
            b.accel.x = (b.vel.x > 0) ? -b.speed * 8 : -b.speed;

            if (b.vel.x < -b.speed)
                b.accel.x = 0;
        }
        else if (controller.state & PLAYER_RIGHT)
        {
            b.accel.x = (b.vel.x < 0) ? b.speed * 8 : b.speed;

            if (b.vel.x > b.speed)
                b.accel.x = 0;
        }

        b.vel.x += (b.accel.x * time_data::step);
    }
    else
    {
        if (b.vel.x > 0)
            b.accel.x = -b.speed * 8;
        else if (b.vel.x < 0)
            b.accel.x = b.speed * 8;

        b.vel.x += (b.accel.x * time_data::step);

        if ((b.accel.x > 0 && b.vel.x > 0) || (b.accel.x < 0 && b.vel.x < 0))
            b.vel.x = 0;
    }

    if (!(b.sensor & BOT))
        b.vel.y += (gravity * time_data::step);

    b.pos += (b.vel * time_data::step);

    if ((b.accel.x > 0 || b.vel.x > 0) && b.pos.x - camera.x > ((W / 3) * 2))
    {
        camera.x += abs (b.pos.x - b.prev.x);
    }
    else if ((b.accel.x < 0 || b.vel.x < 0) && b.pos.x - camera.x < (W / 3))
    {
        camera.x -= abs (b.pos.x - b.prev.x);
    }

    if (b.vel.y > 0 && b.pos.y - camera.y > ((H / 3) * 2.3)
        && !(b.sensor & BOT))
    {
        camera.y += abs (b.pos.y - b.prev.y);
    }
    else if (b.vel.y < 0 && b.pos.y - camera.y < (H / 3) && !(b.sensor & TOP))
    {
        camera.y -= abs (b.pos.y - b.prev.y);
    }

    if (camera.y < 0)
        camera.y = 0;
    if (camera.x < 0)
        camera.x = 0;

    b.sensor = NONE;

    push (grid, &b);
}

void move (Body &body)
{
    switch (body.type)
    {
        case PLAYER: move_player (body); break;

        case ENEMY:
            body.vel.y += (gravity * time_data::step);
            body.pos += (body.vel * time_data::step);
            break;

        case PLATFORM: push (grid, &body); break;
    }
}

void update (Body &body)
{
    switch (body.type)
    {
        case PLAYER: break;
        case ENEMY: break;
        case PLATFORM: break;
    }
}

enum TIMER_CONFIG
{
    SIMPLE  = 1,
    TWO_WAY = 2,
    LOOP    = 4,
};

enum TIMER_STATUS
{
    START        = 1,
    WAIT         = 2,
    DONE         = 4,
    JUST_WAITED  = 8,
    JUST_STARTED = 16,
    JUST_DONE    = 32,
};

struct Timer
{
    uint config, delay, restart_delay, current, state;
};

void set (Timer &t, uint state)
{
    t.state   = state;
    t.current = SDL_GetTicks ();
}

Timer update (Timer &t)
{
    uint diff = SDL_GetTicks () - t.current, just_set = NONE;

    if (t.state == NONE)
    {
        set (t, (START | JUST_STARTED));
        just_set = JUST_STARTED;
    }
    else if (t.state & DONE && t.config & LOOP)
    {
        set (t, START);
    }
    else if (t.state & START && diff >= t.delay)
    {
        if (t.config & TWO_WAY)
        {
            set (t, (WAIT | JUST_WAITED));
            just_set = JUST_WAITED;
        }
        else
        {
            set (t, (DONE | JUST_DONE));
            just_set = JUST_DONE;
        }
    }
    else if (t.state & WAIT && diff >= t.restart_delay)
    {
        set (t, DONE | JUST_DONE);
        just_set = JUST_DONE;
    }

    if (t.state & JUST_STARTED && !(just_set & JUST_STARTED))
    {
        t.state &= ~JUST_STARTED;
    }

    if (t.state & JUST_WAITED && !(just_set & JUST_WAITED))
    {
        t.state &= ~JUST_WAITED;
    }

    if (t.state & JUST_DONE && !(just_set & JUST_DONE))
    {
        t.state &= ~JUST_DONE;
    }

    return t;
}

struct Animation
{
    uint  current, size;
    Timer timer;

    const vec4 sprites[10];
};

vec4 update (Animation &a)
{
    update (a.timer);

    if (a.timer.state & DONE)
    {
        if (a.current < a.size - 1)
        {
            a.current++;
        }
        else
        {
            a.current = 0;
        }
    }

    return a.sprites[a.current];
}

void draw_player (Body &b)
{
    static Animation standing = {
        0,
        3,
        { SIMPLE | LOOP, 2000, 0, 0, 0 },
        {
            { 0, 0, 16, 16 },
            { 16, 0, 16, 16 },
            { 32, 0, 16, 16 },
        },
    };

    static Animation walking = {
        0,
        3,
        { SIMPLE | LOOP, 150, 0, 0, 0 },
        {
            { 48, 0, 16, 16 },
            { 64, 0, 16, 16 },
            { 80, 0, 16, 16 },
        },
    };

    static Animation jumping = {
        0,
        1,
        { SIMPLE | LOOP, 100, 0, 0, 0 },
        { 96, 0, 16, 16 },
    };

    static Animation hold = {
        0,
        1,
        { SIMPLE | LOOP, 100, 0, 0, 0 },
        { 128, 0, 16, 16 },
    };

    uint       flip = NONE;
    vec4       sprite;
    Animation *animation = 0;

    if (controller.state & PLAYER_LEFT)
        flip |= FLIP_X_FALSE;
    else if (controller.state & PLAYER_RIGHT)
        flip |= FLIP_X_TRUE;

    if (b.sensor & (LEFT | RIGHT))
        animation = &hold;
    else if (!(b.sensor & BOT))
        animation = &jumping;
    else if (flip)
        animation = &walking;
    else
        animation = &standing;

    sprite = update (*animation);

    push (sprites, { b.pos - camera, b.size, sprite, b.angle, flip });
}

void draw (Body &b)
{
    switch (b.type)
    {
        case PLAYER: draw_player (b); break;
        case ENEMY: break;
        case PLATFORM:
            push (squares, { b.pos - camera, b.size, b.sprite, b.angle });
            break;
    }
}

struct aabb
{
    float x, w, y, h;

    static const int offset_x = 4;
    static const int offset_y = 6;
};

bool left (aabb a, aabb b)
{
    float dw = a.x + (((a.w - a.x) / 10) * aabb::offset_x);

    return (b.x >= a.x && b.w <= dw && b.y >= a.y && b.h <= a.h) ? true : false;
}

bool right (aabb a, aabb b)
{
    float dx = a.x + (((a.w - a.x) / 10) * aabb::offset_y);

    return (b.x >= dx && b.w <= a.w && b.y >= a.y && b.h <= a.h) ? true : false;
}

bool top (aabb a, aabb b)
{
    float dh = a.y + (((a.h - a.y) / 10) * aabb::offset_x);

    return (b.x >= a.x && b.w <= a.w && b.y >= a.y && b.h <= dh) ? true : false;
}

bool bot (aabb a, aabb b)
{
    float dy = a.y + (((a.h - a.y) / 10) * aabb::offset_y);

    return (b.x >= a.x && b.w <= a.w && b.y >= dy && b.h <= a.h) ? true : false;
}

void collision_spacing (Body *first, aabb a, aabb c)
{
    if (top (a, c) && first->vel.y < 0)
    {
        first->pos.y += ((c.h - c.y));
        first->sensor |= TOP;
        first->vel.y = 0;
    }
    else if (bot (a, c) && first->vel.y >= 0)
    {
        first->pos.y -= ((c.h - c.y) - 1);
        first->sensor |= BOT;
        first->vel.y = 0;
    }
    else if (left (a, c))
    {
        first->pos.x += ((c.w - c.x));
        first->sensor |= LEFT;
    }
    else if (right (a, c))
    {
        first->pos.x -= ((c.w - c.x));
        first->sensor |= RIGHT;
    }
}

aabb generate_aabb (Body *a)
{
    aabb data;

    switch (a->type)
    {
        case PLAYER:
            data = {
                a->pos.x + 5.f,
                a->pos.x + (a->size.x - 4.f),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;

        case ENEMY:
            data = {
                a->pos.x + 5.f,
                a->pos.x + (a->size.x - 4.f),
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;

        case PLATFORM:
            data = {
                a->pos.x,
                a->pos.x + a->size.x,
                a->pos.y,
                a->pos.y + (a->size.y),
            };
            break;
    }

    return data;
}

void solve (Body *first, Body *second)
{
    aabb a = generate_aabb (first);
    aabb b = generate_aabb (second);

    if (a.x < b.w && a.w > b.x && a.y < b.h && a.h > b.y)
    {
        aabb c = {
            (a.x >= b.x) ? a.x : b.x,
            (a.w <= b.w) ? a.w : b.w,
            (a.y >= b.y) ? a.y : b.y,
            (a.h <= b.h) ? a.h : b.h,
        };

        if (first->config == COLLISION_SPACING)
            collision_spacing (first, a, c);

        if (second->config == COLLISION_SPACING)
            collision_spacing (second, b, c);
    }
}

void check ()
{
    for (Node<Cell> *cell = grid.first; cell != 0; cell = cell->next)
    {
        if (cell->data.bodies.length <= 1)
            continue;

        List<Body *> *bodies = &cell->data.bodies;
        Node<Body *> *cond   = limit (*bodies);

        if (!cond)
            cond = bodies->last;

        for (auto i = bodies->first; i != bodies->last; i = i->next)
        {
            for (auto j = i->next; j != 0; j = j->next)
            {
                if (i->data->type == PLATFORM && j->data->type == PLATFORM)
                    continue;

                solve (i->data, j->data);
            }
        }
    }
}

Body get_body ()
{
    Body b;

    b.speed = b.angle = 0;
    b.sensor = b.config = NONE;
    b.prev = b.accel = b.pos = b.size = b.vel = { 0.f, 0.f };

    b.type = PLATFORM;

    return b;
}

void init_player ()
{
    Body b = get_body ();

    b.pos    = { 160.f, 160.f };
    b.size   = { 16.f, 16.f };
    b.type   = PLAYER;
    b.config = COLLISION_SPACING;
    b.speed  = 200.f;

    push (bodies, b);
}

void add_platform (vec2 pos, vec2 size)
{
    Body b = get_body ();

    b.pos    = pos;
    b.size   = size;
    b.type   = PLATFORM;
    b.sprite = { 0, 0, 1, 1 };

    push (bodies, b);
}

const vec4 sprite_coords[] = {
    { 0, 0, 16, 16 },
    { 0, 32, 16, 16 },
    { 0, 64, 16, 16 },
};

const int sprite_coords_size = sizeof (sprite_coords) / sizeof (vec4);

void draw_grid (vec2 mouse, int index)
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

    push (sprites, { mouse, { 16, 16 }, sprite_coords[index], 0, true });
    push (squares, { mouse, { 16, 16 }, { 1, 1, 0, 1 }, 0, true });
}

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
    SDL_Init (SDL_INIT_EVERYTHING);

    time_data::init ();

    bool        run = true;
    SDL_Event   event;
    SDL_Window *window
        = SDL_CreateWindow ("fair_challenge", 0, 0, W * 3, H * 3,
                            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_GL_CreateContext (window);

    SDL_GL_SetSwapInterval (0);

    glewInit ();
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader shader       = init_default_shader ();
    Shader light_shader = init_lighting_shader ();

    // glUseProgram (shader.id);

    Texture spritesheet = load_xpm (spritesheet_xpm);

    // glActiveTexture (GL_TEXTURE0);
    // glBindTexture (GL_TEXTURE_2D, spritesheet.id);

    add_platform ({ 0, 0 }, { 320, 8 });
    add_platform ({ 0, 312 }, { 1280, 8 });

    add_platform ({ 0, 0 }, { 8, 320 });
    add_platform ({ 312, 0 }, { 8, 295 });

    init_player ();

    vec2 mouse        = { 0, 0 };
    int  sprite_index = 0;

    init_frame_buffer (window);

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
                update_aspect_ratio (window, W, H);
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

                        if (sprite_index >= sprite_coords_size)
                            sprite_index = 0;
                    }
                    else
                    {
                        sprite_index--;

                        if (sprite_index < 0)
                            sprite_index = sprite_coords_size - 1;
                    }
                }
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

        if (editor)
            draw_grid (mouse, sprite_index);

        // glClearColor (0.08f, 0.08f, 0.08f, 1.f);
        // glClear (GL_COLOR_BUFFER_BIT);

        // for (auto i = squares.first; i != limit (squares); i = i->next)
        // {
        //     draw (shader, i->data);
        // }

        // for (auto i = sprites.first; i != limit (sprites); i = i->next)
        // {
        //     draw (shader, spritesheet, i->data);
        // }

        // SDL_GL_SwapWindow (window);

        // reset (squares);
        // reset (sprites);

        batch_render (window, light_shader, shader, spritesheet);

        update (controller);
    }

    return 0;
}
