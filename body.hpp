#ifndef STRUCTS
#define STRUCTS

#include "globals.hpp"
#include "math_data.hpp"

struct Body
{
    uint  sensor, config;
    vec2  pos, size, vel;
    float speed, angle;

    uint type;

    vec4 sprite;
};

#endif
