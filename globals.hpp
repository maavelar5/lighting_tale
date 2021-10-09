#ifndef GLOBALS
#define GLOBALS

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

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

const float W = 320.f;
const float H = 320.f;

const int MAX_LIGHTS = 120;

vec2 camera = { 0.f, 0.f };

bool maintain_aspect_ratio = false;

const float gravity = 800.f;

bool editor          = false;
bool collision_boxes = true;

#endif
