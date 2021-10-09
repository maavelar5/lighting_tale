#ifndef LEVELS
#define LEVELS
#include "editor.hpp"
void init_levels () {
Level *entities = 0;
add_level();
entities = &levels.last->data;
push(*entities, {{0.0f, 0.0f}, {320.0f, 8.0f}, (BODY_TYPES)3});
push(*entities, {{0.0f, 312.0f}, {1280.0f, 8.0f}, (BODY_TYPES)3});
push(*entities, {{0.0f, 0.0f}, {8.0f, 320.0f}, (BODY_TYPES)3});
push(*entities, {{312.0f, 0.0f}, {8.0f, 295.0f}, (BODY_TYPES)3});
push(*entities, {{144.0f, 144.0f}, {16.0f, 16.0f}, (BODY_TYPES)3});
push(*entities, {{240.0f, 240.0f}, {16.0f, 160.0f}, (BODY_TYPES)3});
push(*entities, {{200.0f, 210.0f}, {100.0f, 64.0f}, (BODY_TYPES)3});
push(*entities, {{0.0f, 0.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(*entities, {{304.0f, 0.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(*entities, {{0.0f, 304.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(*entities, {{304.0f, 304.0f}, {16.0f, 16.0f}, (BODY_TYPES)0});
}

#endif