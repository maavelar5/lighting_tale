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

template <class T> struct Node
{
    T     data;
    Node *next, *prev;

    Node (T data)
    {
        this->data = data;
        next = prev = 0;
    }

    T &operator-> ()
    {
        return &data;
    }
};

template <class T> struct List
{
    Node<T> *first, *last, *current;
    size_t   size, length;

    List ()
    {
        current = first = last = 0;
        length = size = 0;
    }

    T &operator[] (size_t i)
    {
        assert (first != 0);

        size_t index = 0;

        for (auto node = first; node != 0; node = node->next, index++)
        {
            if (i == index)
                return node->data;
        }

        return last->data;
    }
};

template <class T> T &push (List<T> &list, T data)
{
    if (list.length != list.size)
    {
        list.length++;

        list.current->data = data;

        if (list.length < list.size)
        {
            list.current = list.current->next;
            return list.current->prev->data;
        }
        else
        {
            return list.current->data;
        }
    }
    else
    {
        Node<T> *node = new Node<T> (data);

        if (!list.first)
        {
            list.first = node;
        }
        else
        {
            node->prev      = list.last;
            list.last->next = node;
        }

        list.length++;
        list.size++;

        list.current = list.last = node;

        return list.current->data;
    }
}

template <class T> Node<T> *limit (List<T> &list)
{
    return (list.size == list.length) ? 0 : list.current;
}

template <class T> void for_each (List<T> &list, void (*fn_ptr) (T &t))
{
    for (Node<T> *i = list.first; i != limit (list); i = i->next)
    {
        fn_ptr (i->data);
    }
}

template <class T> void reset (List<T> &list)
{
    list.current = list.first;
    list.length  = 0;
}

template <class T> void clean (List<T> &list)
{
    for (Node<T> *i = list.first; i != 0;)
    {
        auto tmp = i->next;

        delete (i);

        i = tmp;
    }
}

template <class T> Node<T> *remove (List<T> &list, Node<T> *node)
{
    Node<T> *tmp = node;

    if (node == list.first)
    {
        if (list.first == list.last)
        {
            node = list.first = list.last = 0;
        }
        else
        {
            node = list.first = list.first->next;
            list.first->prev  = 0;
        }
    }
    else if (node == list.last)
    {
        node = list.last = list.last->prev;
        list.last->next  = 0;
    }
    else
    {
        node->prev->next = node->next;
        node->next->prev = node->prev;

        node = node->next;
    }

    list.current = list.last;
    list.length  = --list.size;

    delete tmp;

    return node;
}

template <class T> void copy_to_array (List<T> list, T *arr)
{
    int i = 0;

    for (auto n = list.first; n != 0; n = n->next, i++)
        arr[i] = n->data;
}

char *to_string (List<char> str)
{
    int   i          = 0;
    char *new_string = (char *)malloc (sizeof (char) * str.length + 1);

    for (auto n = str.first; n != limit (str); n = n->next, i++)
    {
        new_string[i] = n->data;
    }

    new_string[i] = '\0';

    return new_string;
}

void to_list (List<char> &list, const char *str)
{
    for (size_t i = 0; i < strlen (str); i++)
        push (list, str[i]);
}

float get_float (char *str)
{
    for (size_t i = 0; i < strlen (str); i++)
    {
    }

    return 0;
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
}

void load_file ()
{
    FILE *  fp;
    char *  line = NULL;
    size_t  len  = 0;
    ssize_t read;

    fp = fopen ("level.hpp", "r");

    if (!fp)
        return;

    const char *to_compare = "add_entity";

    while ((read = getline (&line, &len, fp)) != -1)
    {
        if (read > 20)
        {
            char comparee[10];

            for (int i = 0; i < 10; i++)
                comparee[i] = line[i];

            if (!strcmp (comparee, to_compare))
            {
                size_t current = 0;

                // order matter baby
                vec2 pos  = vec2_from_file (line, current);
                vec2 size = vec2_from_file (line, current);
                int  type = int_from_file (line, current);

                printf ("%.2f, %.2f\n", pos.x, pos.y);
                printf ("%.2f, %.2f\n", size.x, size.y);
                printf ("%d\n", type);
            }
        }
    }

    fclose (fp);
}

#endif
