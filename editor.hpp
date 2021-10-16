#ifndef EDITOR
#define EDITOR

#include "body.hpp"
#include "entities.hpp"
#include "globals.hpp"
#include "utils.hpp"

struct Entity
{
    vec2       pos, size;
    BODY_TYPES type;

    NODE_PROPERTIES (Entity);
};

struct Level
{
    NODE_PROPERTIES (Level);
    LIST_PROPERTIES (Entity);
};

struct Level_List
{
    LIST_PROPERTIES (Level);
};

COMMON_FUNCTIONS (Entity, Level);
COMMON_FUNCTIONS (Level, Level_List);

Level *    current_level;
Level_List levels;

inline void save_level (FILE *fp, Level *entities)
{
    if (!entities->size)
        return;

    char  line[200];
    char *text = (char *)malloc (sizeof (char) * 40000);

    strcpy (text, "add_level();\n");
    strcat (text, "entities = levels.last;\n");

    for (auto n = entities->first; n != limit (*entities); n = n->next)
    {
        sprintf (line,
                 "push(entities, {{%.1ff, %.1ff}, {%.1ff, %.1ff}, "
                 "(BODY_TYPES)%d});\n",
                 n->pos.x, n->pos.y, n->size.x, n->size.y, n->type);

        strcat (text, line);
    }

    fputs (text, fp);
    free (text);
}

void save_levels ()
{
    char *text = (char *)malloc (sizeof (char) * 500);

    strcpy (text, "#ifndef LEVELS\n#define LEVELS\n#include "
                  "\"editor.hpp\"\nvoid init_levels () {\n");

    strcat (text, "Level *entities = 0;\n");

    FILE *fp = fopen ("level.hpp", "w");

    if (!fp)
        return;

    fputs (text, fp);

    for (auto n = levels.first; n != limit (levels); n = n->next)
    {
        save_level (fp, n);
    }

    strcpy (text, "}\n\n#endif\n");
    fputs (text, fp);

    fclose (fp);
    free (text);
}

inline void add_level ()
{
    push (&levels, { 0 });

    current_level = levels.last;
}

inline void update_entities ()
{
    reset (&bodies);
    reset (&lights);

    for (auto n = levels.first; n != limit (levels); n = n->next)
    {
        for (auto e = n->first; e != limit (*n); e = e->next)
        {
            switch (e->type)
            {
                case FIRE: add_fire (e->pos, e->size); break;
                case PLAYER: add_player (e->pos, e->size); break;
                case PLATFORM: add_platform (e->pos, e->size); break;
                case ENEMY: add_enemy (e->pos, e->size); break;
                default: break;
            }
        }
    }

    save_levels ();
}

inline void add_entity (vec2 pos, vec2 size, BODY_TYPES type)
{
    push (current_level, { pos, size, type });

    update_entities ();
}

inline void remove_entity (vec2 pos)
{
    pos += camera;

    for (auto n = current_level->first; n != limit (*current_level);)
    {
        if ((pos.x >= n->pos.x && pos.x <= (n->pos.x + n->size.x))
            && (pos.y >= n->pos.y && pos.y <= (n->pos.y + n->size.y)))
            n = remove (current_level, n);
        else
            n = n->next;
    }

    update_entities ();
}

#endif
