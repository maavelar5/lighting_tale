#ifndef CELL
#define CELL

#include "globals.hpp"
#include "utils.hpp"

struct BodyPTR
{
    Body *body;
    NODE_PROPERTIES (BodyPTR);
};

struct BodyPTRS
{
    LIST_PROPERTIES (BodyPTR);
};

struct Cell
{
    int      x, y;
    BodyPTRS bodies;

    NODE_PROPERTIES (Cell)
};

struct Grid
{
    LIST_PROPERTIES (Cell)
};

COMMON_FUNCTIONS (BodyPTR, BodyPTRS);

inline Cell *push (Grid *list, Cell data) PUSH (Cell);
inline Cell *limit (Grid list) LIMIT;

inline Cell *find (Grid *list, Cell cell)
{
    for (Cell *i = list->first; i != limit (*list); i = i->next)
        if (i->x == cell.x && i->y == cell.y)
            return i;

    return push (list, cell);
}

inline void reset (Grid *list)
{
    for (Cell *i = list->first; i != limit (*list); i = i->next)
        reset (&i->bodies);

    RESET;
}

Grid grid = { 0, 0, 0, 0, 0 };

#endif
