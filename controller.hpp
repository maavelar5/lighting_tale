#ifndef CONTROLLER
#define CONTROLLER

#include "globals.hpp"
#include "utils.hpp"

enum INPUT_TYPE
{
    CLICK   = SDL_MOUSEBUTTONDOWN,
    MOTION  = SDL_MOUSEMOTION,
    BUTTON  = SDL_CONTROLLERBUTTONDOWN,
    KEYDOWN = SDL_KEYDOWN,
};

struct Input
{
    int        data, action;
    INPUT_TYPE type;

    uint time;
    bool active;

    NODE_PROPERTIES (Input)
};

struct Input_List
{
    LIST_PROPERTIES (Input);
};

COMMON_FUNCTIONS (Input, Input_List);

struct Controller
{
    uint       state;
    Input_List map, inputs;
};

Input *find (Input_List map, int data, int type)
{
    for (Input *i = map.first; i != limit (map); i = i->next)
        if (i->data == data && i->type == type)
            return i;

    return 0;
}

Input *find (Input_List inputs, int action)
{
    for (Input *i = inputs.first; i != limit (inputs); i = i->next)
        if (i->action == action)
            return i;

    return 0;
}

bool just_pressed (Input_List inputs, int action, bool uncheck = false)
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
        return false;
}

void remove (Input_List *inputs, Input value)
{
    for (Input *i = inputs->first; i != limit (*inputs);)
        if (i->action == value.action && i->type == value.type)
            i = remove (inputs, i);
        else
            i = i->next;
}

void set (Controller *controller, int data, int type)
{
    Input *input = find (controller->map, data, type);

    if (input)
    {
        input->time   = SDL_GetTicks ();
        input->active = true;
        controller->state |= input->action;

        push (&controller->inputs, *input);
    }
}

void unset (Controller *controller, int data, int type)
{
    Input *input = find (controller->inputs, data, type);

    if (input)
        input->active = false;
}

void update (Controller *controller)
{
    for (auto i = controller->inputs.first; i != limit (controller->inputs);)
    {
        if (!i->active && SDL_GetTicks () - i->time > 100)
        {
            controller->state &= ~i->action;
            i = remove (&controller->inputs, i);
        }
        else
        {
            i = i->next;
        }
    }
}

#endif
