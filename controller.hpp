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
    PLAYER_JUMP,
    PLAYER_LEFT,
    PLAYER_RIGHT,
    PLAYER_DASH,

    // Editor
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,

    INCREASE_H,
    INCREASE_W,

    SHOW_EDITOR,
    HIDE_EDITOR,

    INCREASE_SIZE_W,
    INCREASE_SIZE_H,

    DECREASE_SIZE_W,
    DECREASE_SIZE_H,

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
    uint  event, action, type;
    bool  active;
    Timer timer;

    NODE_PROPERTIES (Input);
};

struct Inputs
{
    LIST_PROPERTIES (Input)
};

COMMON_FUNCTIONS (Input, Inputs)

struct Controller
{
    Inputs map, inputs;
};

Input *find (Inputs map, uint event, uint type)
{
    for (Input *i = map.first; i != limit (map); i = i->next)
        if (i->event == event && i->type == type)
            return i;

    return 0;
}

Input *find_by_event (Inputs map, uint event)
{
    for (Input *i = map.first; i != limit (map); i = i->next)
        if (i->event == event)
            return i;

    return 0;
}

Input *find (Inputs map, uint action)
{
    for (Input *i = map.first; i != limit (map); i = i->next)
        if (i->action == action)
            return i;

    return 0;
}

Input *active (Inputs inputs, uint action)
{
    Input *input = find (inputs, action);

    return (input && input->timer.state & START) ? input : 0;
}

void remove (Inputs *inputs, uint action)
{
    for (Input *i = inputs->first; i != limit (*inputs);)
        if (i->action == action)
            i = remove (inputs, i);
        else
            i = i->next;
}

void set (Controller *controller, uint event, uint type)
{
    Input *input = find_by_event (controller->map, event);

    if (input)
    {
        input->active = true;
        input->timer  = INPUT_TIMER (SIMPLE);

        push (&controller->inputs, *input);
    }
}

void unset (Controller *controller, uint event, uint type)
{
    Input *input = find (controller->inputs, event, type);

    if (input)
        input->active = false;
}

void update (Controller *controller)
{
    for (auto i = controller->inputs.first; i != limit (controller->inputs);)
    {
        update (&i->timer);

        if (!i->active && i->timer.state & DONE)
            i = remove (&controller->inputs, i);
        else
            i = i->next;
    }
}

#endif
