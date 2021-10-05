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

    glViewport (w / 2, h / 2, ratio, ratio);
}

inline mat4 get_model (vec2 pos, vec2 size)
{
    mat4 matrix = identity ();

    translate (matrix, pos);
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
};

template <class T> T &push (List<T> &list, T data)
{
    list.length++;

    if (list.current != list.last)
    {
        list.current->data = data;
        list.current       = list.current->next;

        return list.current->prev->data;
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

#endif
