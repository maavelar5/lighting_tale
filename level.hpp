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
push(entities, {{454.8f, 197.2f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{30.2f, 128.8f}, {99.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{-47.8f, -91.6f}, {567.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{-32.4f, -84.8f}, {33.0f, 405.0f}, (BODY_TYPES)3});
push(entities, {{519.2f, -91.6f}, {353.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{871.8f, -91.6f}, {395.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{1266.4f, -91.1f}, {13.0f, 411.0f}, (BODY_TYPES)3});
push(entities, {{583.4f, 6.2f}, {8.0f, 8.0f}, (BODY_TYPES)0});
push(entities, {{746.2f, -5.8f}, {8.0f, 8.0f}, (BODY_TYPES)0});
push(entities, {{1015.0f, 125.2f}, {8.0f, 8.0f}, (BODY_TYPES)0});
push(entities, {{565.4f, 83.2f}, {37.0f, 57.0f}, (BODY_TYPES)3});
push(entities, {{815.4f, 93.2f}, {47.0f, 51.0f}, (BODY_TYPES)3});
push(entities, {{897.0f, -22.8f}, {35.0f, 55.0f}, (BODY_TYPES)3});
push(entities, {{1025.8f, 179.6f}, {43.0f, 66.0f}, (BODY_TYPES)3});
push(entities, {{1102.4f, -35.8f}, {32.0f, 46.0f}, (BODY_TYPES)3});
push(entities, {{1183.2f, 73.6f}, {41.0f, 62.0f}, (BODY_TYPES)3});
push(entities, {{1179.8f, 177.8f}, {34.0f, 42.0f}, (BODY_TYPES)3});
push(entities, {{915.8f, 189.8f}, {44.0f, 60.0f}, (BODY_TYPES)3});
push(entities, {{1.0f, 0.0f}, {351.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{338.2f, 53.6f}, {43.0f, 40.0f}, (BODY_TYPES)3});
push(entities, {{445.0f, 63.6f}, {36.0f, 46.0f}, (BODY_TYPES)3});
push(entities, {{510.2f, 178.0f}, {47.0f, 42.0f}, (BODY_TYPES)3});
push(entities, {{628.4f, 33.6f}, {25.0f, 34.0f}, (BODY_TYPES)3});
push(entities, {{349.6f, -0.4f}, {70.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{385.2f, -85.4f}, {73.0f, 85.0f}, (BODY_TYPES)3});
push(entities, {{525.2f, -12.2f}, {35.0f, 42.0f}, (BODY_TYPES)3});
push(entities, {{410.2f, -1.0f}, {865.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{992.7f, 242.1f}, {17.0f, 45.0f}, (BODY_TYPES)3});
push(entities, {{895.8f, 114.8f}, {14.0f, 22.0f}, (BODY_TYPES)3});
push(entities, {{949.9f, 138.0f}, {17.0f, 23.0f}, (BODY_TYPES)3});
push(entities, {{664.8f, 170.4f}, {149.0f, 15.0f}, (BODY_TYPES)3});
push(entities, {{657.8f, 98.4f}, {18.0f, 23.0f}, (BODY_TYPES)3});
push(entities, {{751.8f, 62.4f}, {20.0f, 28.0f}, (BODY_TYPES)3});
push(entities, {{708.8f, 115.4f}, {18.0f, 30.0f}, (BODY_TYPES)3});
push(entities, {{333.4f, 206.6f}, {16.0f, 15.0f}, (BODY_TYPES)1});
push(entities, {{386.0f, 221.6f}, {16.0f, 18.0f}, (BODY_TYPES)1});
push(entities, {{461.4f, 151.2f}, {22.0f, 21.0f}, (BODY_TYPES)1});
push(entities, {{579.2f, 41.0f}, {13.0f, 19.0f}, (BODY_TYPES)1});
push(entities, {{414.6f, 33.2f}, {16.0f, 16.0f}, (BODY_TYPES)1});
push(entities, {{1235.8f, 22.8f}, {8.0f, 8.0f}, (BODY_TYPES)0});
}

#endif
