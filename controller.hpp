#ifndef CONTROLLER
#define CONTROLLER

#include "globals.hpp"
#include "timer.hpp"
#include "utils.hpp"

enum INPUT_TYPE
{
    KEY    = SDL_KEYDOWN,
    CLICK  = SDL_MOUSEBUTTONDOWN,
    MOTION = SDL_MOUSEMOTION,
    BUTTON = SDL_CONTROLLERBUTTONDOWN,
};

enum ACTIONS
{
    PLAYER_JUMP = 1,
    PLAYER_LEFT,
    PLAYER_RIGHT,
    PLAYER_DASH,
    PLAYER_ATTACK,
    PLAYER_DOWN,
    PLAYER_UP,

    // Editor
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,

    INCREASE_W,
    INCREASE_H,
    DECREASE_W,
    DECREASE_H,

    SHOW_EDITOR,
    HIDE_EDITOR,

    INCREASE_EDITOR,
    DECREASE_EDITOR,

    TOGGLE_ASPECT_RATIO,
    TOGGLE_COLLISION_BOXES,
};

#define INPUT_TIMER(FLAGS)                                     \
    {                                                          \
        FLAGS, 100, 100, SDL_GetTicks (), START | JUST_STARTED \
    }

struct Input
{
    uint  action;
    bool  active;
    Timer timer;
};

typedef array<Input> Inputs;

bool active (Input *input)
{
    if (!input)
        return false;
    else if (input->timer.state & START)
        return true;
    else
        return false;
}

bool finished (Input *input)
{
    if (!input)
        return false;
    else if (input->timer.state & DONE)
        return true;
    else
        return false;
}

void remove (Inputs &inputs, uint action)
{
    for (int i = 0; i < inputs.length;)
        if (inputs[i].action == action)
            inputs.remove (i);
        else
            i++;
}

void set_inactive (Inputs &inputs, uint action)
{
    for (int i = 0; i < inputs.length; i++)
        if (inputs[i].action == action)
            inputs[i].active = false;
}

void update (Inputs &inputs)
{
    for (int i = 0; i < inputs.length;)
    {
        Input &input = inputs[i];

        update (input.timer);

        if (!input.active && input.timer.state & DONE)
            inputs.remove (i);
        else
            i++;
    }
}

void push (Inputs &inputs, uint action, uint config = SIMPLE)
{
    inputs.push ({ action, true, INPUT_TIMER (config) });
}

#endif
