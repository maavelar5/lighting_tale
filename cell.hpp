#ifndef CELL
#define CELL

#include "globals.hpp"
#include "utils.hpp"

struct Cell
{
    int           x, y;
    array<Body *> bodies;
};

typedef array<Cell> Grid;

inline Cell &find (Grid &grid, Cell cell)
{
    for (int i = 0; i < grid.length; i++)
        if (grid[i].x == cell.x && grid[i].y == cell.y)
            return grid[i];

    return grid.push (cell);
}

inline void reset (Grid &grid)
{
    for (int i = 0; i < grid.length; i++)
        free (grid[i].bodies.data);

    grid.length = 0;
}

Grid grid;

#endif
