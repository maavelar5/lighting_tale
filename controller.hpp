#ifndef CONTROLLER
#define CONTROLLER

#include "globals.hpp"
#include "utils.hpp"

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
    PLAYER_DASH  = 128,

    TOGGLE_EDITOR       = 256,
    TOGGLE_ASPECT_RATIO = 512,
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

Controller get_controller ()
{
    Controller controller;

    push (controller.map, { SDLK_d, PLAYER_RIGHT, KEYDOWN });
    push (controller.map, { SDLK_a, PLAYER_LEFT, KEYDOWN });
    push (controller.map, { SDLK_j, PLAYER_DASH, KEYDOWN });
    push (controller.map, { SDLK_SPACE, PLAYER_JUMP, KEYDOWN });

    push (controller.map, { SDLK_F1, TOGGLE_EDITOR, KEYDOWN });
    push (controller.map, { SDLK_F2, TOGGLE_ASPECT_RATIO, KEYDOWN });

    return controller;
}

Controller controller = get_controller ();

#endif
