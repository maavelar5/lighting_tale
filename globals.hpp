#ifndef GLOBALS
#define GLOBALS

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

const float W = 320.f;
const float H = 320.f;

#endif
