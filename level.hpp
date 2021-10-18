#ifndef LEVELS
#define LEVELS
#include "editor.hpp"
void init_levels () {
Level *entities = 0;
add_level();
entities = levels.last;
push(entities, {{200.0f, 210.0f}, {100.0f, 64.0f}, (BODY_TYPES)3});
push(entities, {{64.0f, 64.0f}, {16.0f, 16.0f}, (BODY_TYPES)2});
push(entities, {{627.6f, 212.1f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{858.0f, 226.1f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{1147.5f, 227.1f}, {16.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{30.2f, 128.8f}, {99.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{565.4f, 83.2f}, {37.0f, 57.0f}, (BODY_TYPES)3});
push(entities, {{815.4f, 93.2f}, {47.0f, 51.0f}, (BODY_TYPES)3});
push(entities, {{1025.8f, 179.6f}, {43.0f, 66.0f}, (BODY_TYPES)3});
push(entities, {{1183.2f, 73.6f}, {41.0f, 62.0f}, (BODY_TYPES)3});
push(entities, {{1179.8f, 177.8f}, {34.0f, 42.0f}, (BODY_TYPES)3});
push(entities, {{915.8f, 189.8f}, {44.0f, 60.0f}, (BODY_TYPES)3});
push(entities, {{338.2f, 53.6f}, {43.0f, 40.0f}, (BODY_TYPES)3});
push(entities, {{445.0f, 63.6f}, {36.0f, 46.0f}, (BODY_TYPES)3});
push(entities, {{510.2f, 178.0f}, {47.0f, 42.0f}, (BODY_TYPES)3});
push(entities, {{628.4f, 33.6f}, {25.0f, 34.0f}, (BODY_TYPES)3});
push(entities, {{992.7f, 242.1f}, {17.0f, 45.0f}, (BODY_TYPES)3});
push(entities, {{895.8f, 114.8f}, {14.0f, 22.0f}, (BODY_TYPES)3});
push(entities, {{949.9f, 138.0f}, {17.0f, 23.0f}, (BODY_TYPES)3});
push(entities, {{664.8f, 170.4f}, {149.0f, 15.0f}, (BODY_TYPES)3});
push(entities, {{657.8f, 98.4f}, {18.0f, 23.0f}, (BODY_TYPES)3});
push(entities, {{751.8f, 62.4f}, {20.0f, 28.0f}, (BODY_TYPES)3});
push(entities, {{708.8f, 115.4f}, {18.0f, 30.0f}, (BODY_TYPES)3});
push(entities, {{396.6f, 173.0f}, {29.0f, 25.0f}, (BODY_TYPES)0});
push(entities, {{-33.0f, -50.8f}, {1313.0f, 21.0f}, (BODY_TYPES)3});
push(entities, {{742.6f, -10.0f}, {26.0f, 28.0f}, (BODY_TYPES)0});
push(entities, {{1232.3f, 16.4f}, {14.0f, 16.0f}, (BODY_TYPES)0});
push(entities, {{1008.5f, 116.0f}, {25.0f, 22.0f}, (BODY_TYPES)0});
push(entities, {{578.0f, 1.0f}, {19.0f, 17.0f}, (BODY_TYPES)0});
push(entities, {{137.4f, 149.3f}, {35.0f, 31.0f}, (BODY_TYPES)3});
push(entities, {{247.7f, 280.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{226.7f, 286.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{262.7f, 293.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{302.7f, 290.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{183.7f, 290.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{165.7f, 273.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{176.7f, 251.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{308.7f, 240.3f}, {8.0f, 8.0f}, (BODY_TYPES)3});
push(entities, {{-94.9f, 393.1f}, {1425.0f, 36.0f}, (BODY_TYPES)3});
push(entities, {{101.1f, 321.1f}, {299.0f, 17.0f}, (BODY_TYPES)3});
push(entities, {{432.1f, 294.1f}, {62.0f, 15.0f}, (BODY_TYPES)3});
push(entities, {{538.1f, 309.1f}, {59.0f, 16.0f}, (BODY_TYPES)3});
push(entities, {{687.1f, 290.1f}, {68.0f, 14.0f}, (BODY_TYPES)3});
push(entities, {{622.1f, 267.1f}, {31.0f, 14.0f}, (BODY_TYPES)3});
push(entities, {{835.1f, 298.1f}, {25.0f, 23.0f}, (BODY_TYPES)3});
push(entities, {{1052.1f, 308.1f}, {24.0f, 23.0f}, (BODY_TYPES)3});
push(entities, {{900.1f, 319.1f}, {44.0f, 36.0f}, (BODY_TYPES)3});
push(entities, {{990.1f, 349.1f}, {26.0f, 16.0f}, (BODY_TYPES)3});
push(entities, {{1111.0f, 326.2f}, {123.0f, 24.0f}, (BODY_TYPES)3});
push(entities, {{-33.1f, -29.0f}, {27.0f, 422.0f}, (BODY_TYPES)3});
push(entities, {{159.0f, 38.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{188.0f, 79.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{200.0f, 117.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{200.0f, 117.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{200.0f, 115.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{208.0f, 133.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{208.0f, 133.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
push(entities, {{1278.1f, -50.8f}, {52.0f, 480.0f}, (BODY_TYPES)3});
push(entities, {{72.1f, 33.0f}, {10.0f, 12.0f}, (BODY_TYPES)0});
push(entities, {{223.1f, 142.0f}, {15.0f, 21.0f}, (BODY_TYPES)0});
push(entities, {{318.1f, 158.0f}, {13.0f, 17.0f}, (BODY_TYPES)0});
push(entities, {{159.1f, 248.0f}, {15.0f, 12.0f}, (BODY_TYPES)0});
}

#endif
