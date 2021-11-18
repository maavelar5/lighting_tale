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
};

typedef array<Entity> Level;
typedef array<Level>  Levels;

Level *current_level;
Levels levels;

inline void save_level (FILE *fp, Level &entities)
{
    if (!entities.size)
        return;

    char  line[200];
    char *text = (char *)malloc (sizeof (char) * 40000);

    strcpy (text, "add_level();\n");
    strcat (text, "entities = &levels.last();\n");

    for (int i = 0; i < entities.length; i++)
    {
        sprintf (line,
                 "entities->push({{%.1ff, %.1ff}, {%.1ff, %.1ff}, "
                 "(BODY_TYPES)%d});\n",
                 entities[i].pos.x, entities[i].pos.y, entities[i].size.x,
                 entities[i].size.y, entities[i].type);

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

    for (int i = 0; i < levels.length; i++)
    {
        save_level (fp, levels[i]);
    }

    strcpy (text, "}\n\n#endif\n");
    fputs (text, fp);

    fclose (fp);
    free (text);
}

inline void add_level ()
{
    levels.push ({});

    current_level = &levels.last ();
}

inline void update_entities ()
{
    bodies.length = lights.length = 0;

    for (int i = 0; i < levels.length; i++)
    {
        for (int j = 0; j < levels[i].length; j++)
        {
            Entity *e = &levels[i][j];

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
    current_level->push ({ pos, size, type });

    update_entities ();
}

inline void remove_entity (vec2 pos)
{
    pos += camera;

    for (int i = 0; i < current_level->length;)
    {
        Entity *n = &current_level->at (i);

        if ((pos.x >= n->pos.x && pos.x <= (n->pos.x + n->size.x))
            && (pos.y >= n->pos.y && pos.y <= (n->pos.y + n->size.y)))
            current_level->remove (i);
        else
            i++;
    }

    update_entities ();
}

#endif
