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

typedef List<Entity> Level;

Level *     current_level;
List<Level> levels;

inline void save_level (FILE *fp, Level &entities)
{
    if (!entities.size)
        return;

    char  line[200];
    char *text = (char *)malloc (sizeof (char) * 40000);

    strcpy (text, "add_level();\n");
    strcat (text, "entities = &levels.last->data;\n");

    for (auto n = entities.first; n != limit (entities); n = n->next)
    {
        sprintf (line,
                 "push(*entities, {{%.1ff, %.1ff}, {%.1ff, %.1ff}, "
                 "(BODY_TYPES)%d});\n",
                 n->data.pos.x, n->data.pos.y, n->data.size.x, n->data.size.y,
                 n->data.type);

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
        save_level (fp, n->data);
    }

    strcpy (text, "}\n\n#endif\n");
    fputs (text, fp);

    fclose (fp);
    free (text);
}

inline void add_level ()
{
    push (levels, {});

    current_level = &levels.last->data;
}

inline void update_entities ()
{
    reset (bodies);
    reset (lights);

    for (auto n = levels.first; n != limit (levels); n = n->next)
    {
        for (auto e = n->data.first; e != limit (n->data); e = e->next)
        {
            switch (e->data.type)
            {
                case FIRE: add_fire (e->data.pos, e->data.size); break;
                case PLAYER: add_player (e->data.pos, e->data.size); break;
                case PLATFORM: add_platform (e->data.pos, e->data.size); break;
                default: break;
            }
        }
    }

    save_levels ();
}

inline void add_entity (vec2 pos, vec2 size, BODY_TYPES type)
{
    push (*current_level, { pos, size, type });

    update_entities ();
}

inline void remove_entity (vec2 pos)
{
    pos += camera;

    for (auto n = current_level->first; n != limit (*current_level);)
    {
        Entity b = n->data;

        if ((pos.x >= b.pos.x && pos.x <= (b.pos.x + b.size.x))
            && (pos.y >= b.pos.y && pos.y <= (b.pos.y + b.size.y)))
            n = remove (*current_level, n);
        else
            n = n->next;
    }

    update_entities ();
}

#endif
