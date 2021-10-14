#ifndef LEVELS
#define LEVELS
#include "editor.hpp"
void init_levels () {
Level *entities = 0;
add_level();
entities = levels.last;
push(entities, {{0.0f, 312.0f}, {1280.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{240.0f, 240.0f}, {16.0f, 160.0f}, (BODY_TYPES)3});
push(entities, {{200.0f, 210.0f}, {100.0f, 64.0f}, (BODY_TYPES)3});
push(entities, {{64.0f, 64.0f}, {16.0f, 16.0f}, (BODY_TYPES)2});
push(entities, {{347.5f, 131.2f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{627.6f, 212.1f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{858.0f, 226.1f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{1147.5f, 227.1f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{144.0f, 147.1f}, {16.0f, 16.0f}, (BODY_TYPES)3});
push(entities, {{277.0f, 157.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{72.0f, 58.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{277.1f, 40.6f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{153.9f, 115.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{454.8f, 197.2f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{30.2f, 128.8f}, {99.0f, 8.0f}, (BODY_TYPES)3});
}

#endif
