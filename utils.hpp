#ifndef UTILS
#define UTILS

#include "globals.hpp"
#include "math_data.hpp"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

inline char *read_file (const char *filename)
{
    char * str;
    size_t pos;

    FILE *file = fopen (filename, "r");

    assert (file != NULL);

    fseek (file, 0, SEEK_END);

    pos = ftell (file);
    str = (char *)malloc (sizeof (char) * pos + 1);

    fseek (file, 0, SEEK_SET);
    fread (str, sizeof (char), pos, file);

    str[pos] = '\0';

    fclose (file);

    return str;
}

inline void update_aspect_ratio (SDL_Window *window, const float W,
                                 const float H)
{
    int w, h, ratio;

    SDL_GL_GetDrawableSize (window, &w, &h);

    if (w > h)
        ratio = (int)(h / H) * H;
    else if (h > w || h == w)
        ratio = (int)(w / W) * W;

    w -= ratio;
    h -= ratio;

    glViewport (0, 0, ratio, ratio);
    // glViewport (w / 2, h / 2, ratio, ratio);
}

inline mat4 get_model (vec2 pos, vec2 size, float angle = 0)
{
    mat4 matrix = identity ();

    translate (matrix, pos);
    translate (matrix, size * (0.5f));

    rotate (matrix, angle);
    translate (matrix, size * (-0.5f));

    scale (matrix, size);

    return matrix;
}

vec2 vec2_from_file (char *line, size_t &current)
{
    vec2 value = { -1337, -1337 };
    int  index = 0;
    char parse[100];

    for (; current < strlen (line); current++)
    {
        if (line[current] >= '0' && line[current] <= '9')
        {
            parse[index++] = line[current];
        }
        else if (line[current] == ',' || line[current] == '}')
        {
            if (index == 0)
                continue;

            parse[index] = '\0';

            if (value.x == -1337)
            {
                value.x = strtol (parse, NULL, 10);
                index   = 0;
            }
            else
            {
                value.y = strtol (parse, NULL, 10);
                current++;
                return value;
            }
        }
        else if (line[current] == ' ')
            continue;
        else
            index = 0;
    }

    assert (1 != 1);
    return { 0, 0 };
}

int int_from_file (char *line, size_t &current)
{
    int  index = 0;
    char parse[100];

    for (; current < strlen (line); current++)
    {
        if (line[current] >= '0' && line[current] <= '9')
        {
            parse[index++] = line[current];
        }
        else if (index > 0)
        {
            parse[index] = '\0';

            return strtol (parse, NULL, 10);
        }
        else if (line[current] == ' ')
            continue;
    }

    assert (1 != 1);

    return -1;
}

void load_file ()
{
    // FILE *  fp;
    // char *  line = NULL;
    // size_t  len  = 0;
    // ssize_t read;

    // fp = fopen ("level.hpp", "r");

    // if (!fp)
    //     return;

    // const char *to_compare = "add_entity";

    // while ((read = getline (&line, &len, fp)) != -1)
    // {
    //     if (read > 20)
    //     {
    //         char comparee[10];

    //         for (int i = 0; i < 10; i++)
    //             comparee[i] = line[i];

    //         if (!strcmp (comparee, to_compare))
    //         {
    //             size_t current = 0;

    //             // order matter baby
    //             vec2 pos  = vec2_from_file (line, current);
    //             vec2 size = vec2_from_file (line, current);
    //             int  type = int_from_file (line, current);

    //             printf ("%.2f, %.2f\n", pos.x, pos.y);
    //             printf ("%.2f, %.2f\n", size.x, size.y);
    //             printf ("%d\n", type);
    //         }
    //     }
    // }

    // fclose (fp);
}

#endif
