#ifndef GLOBALS
#define GLOBALS

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "math_data.hpp"

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
