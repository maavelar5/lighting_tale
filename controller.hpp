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

Input *find (List<Input> map, int data, int type)
{
    for (auto i = map.first; i != limit (map); i = i->next)
        if (i->data.data == data && i->data.type == type)
            return &i->data;

    return 0;
}

Input *find (List<Input> inputs, int action)
{
    for (auto i = inputs.first; i != limit (inputs); i = i->next)
        if (i->data.action == action)
            return &i->data;

    return 0;
}

bool just_pressed (List<Input> inputs, int action, bool uncheck = false)
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

void remove (List<Input> &inputs, Input value)
{
    for (auto i = inputs.first; i != limit (inputs);)
        if (i->data == value)
            i = remove (inputs, i);
        else
            i = i->next;
}

void set (Controller &controller, int data, int type)
{
    Input *input = find (controller.map, data, type);

    if (input)
    {
        input->time   = SDL_GetTicks ();
        input->active = true;
        controller.state |= input->action;

        push (controller.inputs, *input);
    }
}

void unset (Controller &controller, int data, int type)
{
    Input *input = find (controller.inputs, data, type);

    if (input)
        input->active = false;
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

#endif
