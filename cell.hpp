#ifndef CELL
#define CELL

#include "globals.hpp"
#include "utils.hpp"

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
    for (Node<Cell> *i = grid.first; i != limit (grid); i = i->next)
        if (i->data == cell)
            return i->data;

    return push (grid, cell);
}

void reset (List<Cell> &grid)
{
    for (Node<Cell> *i = grid.first; i != limit (grid); i = i->next)
        reset (i->data.bodies);

    grid.current = grid.first;
    grid.length  = 0;
}

List<Cell> grid;

#endif
