#ifndef GLOBALS
#define GLOBALS

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "math_data.hpp"

#define NODE_PROPERTIES(type) type *next, *prev;

#define LIST_PROPERTIES(type)      \
    type * first, *last, *current; \
    size_t length, size;

#define PUSH(Type)                                       \
    {                                                    \
        if (list->length < list->size)                   \
        {                                                \
            list->length++;                              \
                                                         \
            Type *next = list->current->next;            \
            Type *prev = list->current->prev;            \
                                                         \
            *list->current      = data;                  \
            list->current->next = next;                  \
            list->current->prev = prev;                  \
                                                         \
            if (list->length < list->size)               \
            {                                            \
                list->current = list->current->next;     \
                return list->current->prev;              \
            }                                            \
            else                                         \
            {                                            \
                return list->current;                    \
            }                                            \
        }                                                \
        else                                             \
        {                                                \
            Type *node = (Type *)malloc (sizeof (Type)); \
                                                         \
            *node      = data;                           \
            node->next = node->prev = 0;                 \
                                                         \
            if (!list->first)                            \
            {                                            \
                list->first = node;                      \
            }                                            \
            else                                         \
            {                                            \
                node->prev       = list->last;           \
                list->last->next = node;                 \
            }                                            \
                                                         \
            list->length  = ++list->size;                \
            list->current = list->last = node;           \
                                                         \
            return list->last;                           \
        }                                                \
    }

#define DELETE(Type)                                    \
    {                                                   \
        Type *tmp = node;                               \
        if (node == list->first)                        \
        {                                               \
            if (list->first == list->last)              \
            {                                           \
                node = list->first = list->last = 0;    \
            }                                           \
            else                                        \
            {                                           \
                node = list->first = list->first->next; \
                list->first->prev  = 0;                 \
            }                                           \
        }                                               \
        else if (node == list->last)                    \
        {                                               \
            node = list->last = list->last->prev;       \
            list->last->next  = 0;                      \
        }                                               \
        else                                            \
        {                                               \
            node->prev->next = node->next;              \
            node->next->prev = node->prev;              \
            node             = node->next;              \
        }                                               \
        list->current = list->last;                     \
        list->length  = --list->size;                   \
        delete tmp;                                     \
        return node;                                    \
    }

#define LIMIT                              \
    {                                      \
        if (list.current == 0)             \
            return list.first;             \
        else if (list.size == list.length) \
            return 0;                      \
        else                               \
            return list.current;           \
    }

#define RESET                        \
    {                                \
        list->length  = 0;           \
        list->current = list->first; \
    }

#define COMMON_FUNCTIONS(Type, List)                       \
    inline Type *push (List *list, Type data) PUSH (Type); \
    inline Type *limit (List list) LIMIT;                  \
    inline void  reset (List *list) RESET;                 \
    inline Type *remove (List *list, Type *node) DELETE (Type);

typedef GLuint   uint;
typedef SDL_Rect ivec4;

const uint NONE = 0;

enum SENSORS
{
    TOP   = 1,
    BOT   = 2,
    LEFT  = 4,
    RIGHT = 8
};

struct Body;

float W = 480.f;
float H = 270.f;

const int MAX_LIGHTS = 120;

vec2 camera = { 0.f, 0.f };

const float gravity = 800.f;

bool run                   = true;
bool maintain_aspect_ratio = false;

#endif
