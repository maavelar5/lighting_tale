#ifndef LEVELS
#define LEVELS
#include "editor.hpp"
void init_levels () {
Level *entities = 0;
add_level();
entities = &levels.last();
entities->push({{64.0f, 64.0f}, {16.0f, 16.0f}, (BODY_TYPES)2});
entities->push({{30.2f, 128.8f}, {99.0f, 8.0f}, (BODY_TYPES)3});
entities->push({{565.4f, 83.2f}, {37.0f, 57.0f}, (BODY_TYPES)3});
entities->push({{815.4f, 93.2f}, {47.0f, 51.0f}, (BODY_TYPES)3});
entities->push({{1025.8f, 179.6f}, {43.0f, 66.0f}, (BODY_TYPES)3});
entities->push({{1183.2f, 73.6f}, {41.0f, 62.0f}, (BODY_TYPES)3});
entities->push({{1179.8f, 177.8f}, {34.0f, 42.0f}, (BODY_TYPES)3});
entities->push({{915.8f, 189.8f}, {44.0f, 60.0f}, (BODY_TYPES)3});
entities->push({{338.2f, 53.6f}, {43.0f, 40.0f}, (BODY_TYPES)3});
entities->push({{445.0f, 63.6f}, {36.0f, 46.0f}, (BODY_TYPES)3});
entities->push({{510.2f, 178.0f}, {47.0f, 42.0f}, (BODY_TYPES)3});
entities->push({{628.4f, 33.6f}, {25.0f, 34.0f}, (BODY_TYPES)3});
entities->push({{895.8f, 114.8f}, {14.0f, 22.0f}, (BODY_TYPES)3});
entities->push({{949.9f, 138.0f}, {17.0f, 23.0f}, (BODY_TYPES)3});
entities->push({{664.8f, 170.4f}, {149.0f, 15.0f}, (BODY_TYPES)3});
entities->push({{657.8f, 98.4f}, {18.0f, 23.0f}, (BODY_TYPES)3});
entities->push({{751.8f, 62.4f}, {20.0f, 28.0f}, (BODY_TYPES)3});
entities->push({{708.8f, 115.4f}, {18.0f, 30.0f}, (BODY_TYPES)3});
entities->push({{-33.0f, -50.8f}, {1313.0f, 21.0f}, (BODY_TYPES)3});
entities->push({{137.4f, 149.3f}, {35.0f, 31.0f}, (BODY_TYPES)3});
entities->push({{-94.9f, 393.1f}, {1425.0f, 36.0f}, (BODY_TYPES)3});
entities->push({{101.1f, 321.1f}, {299.0f, 17.0f}, (BODY_TYPES)3});
entities->push({{432.1f, 294.1f}, {62.0f, 15.0f}, (BODY_TYPES)3});
entities->push({{538.1f, 309.1f}, {59.0f, 16.0f}, (BODY_TYPES)3});
entities->push({{687.1f, 290.1f}, {68.0f, 14.0f}, (BODY_TYPES)3});
entities->push({{622.1f, 267.1f}, {31.0f, 14.0f}, (BODY_TYPES)3});
entities->push({{835.1f, 298.1f}, {25.0f, 23.0f}, (BODY_TYPES)3});
entities->push({{1052.1f, 308.1f}, {24.0f, 23.0f}, (BODY_TYPES)3});
entities->push({{900.1f, 319.1f}, {44.0f, 36.0f}, (BODY_TYPES)3});
entities->push({{990.1f, 349.1f}, {26.0f, 16.0f}, (BODY_TYPES)3});
entities->push({{1111.0f, 326.2f}, {123.0f, 24.0f}, (BODY_TYPES)3});
entities->push({{-33.1f, -29.0f}, {27.0f, 422.0f}, (BODY_TYPES)3});
entities->push({{159.0f, 38.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{188.0f, 79.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{200.0f, 117.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{200.0f, 117.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{200.0f, 115.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{208.0f, 133.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{208.0f, 133.0f}, {8.0f, 8.0f}, (BODY_TYPES)4});
entities->push({{205.4f, 278.8f}, {14.0f, 13.0f}, (BODY_TYPES)1});
entities->push({{277.4f, 276.8f}, {16.0f, 17.0f}, (BODY_TYPES)1});
entities->push({{103.6f, 196.3f}, {18.0f, 16.0f}, (BODY_TYPES)1});
entities->push({{350.6f, 185.3f}, {22.0f, 20.0f}, (BODY_TYPES)1});
entities->push({{453.6f, -26.6f}, {27.0f, 20.0f}, (BODY_TYPES)1});
entities->push({{1027.6f, 154.0f}, {18.0f, 11.0f}, (BODY_TYPES)1});
entities->push({{811.8f, 60.0f}, {15.0f, 13.0f}, (BODY_TYPES)1});
entities->push({{22.4f, 333.7f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{14.5f, -15.7f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{166.2f, 84.5f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{248.4f, 143.7f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{1199.1f, 23.5f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{421.0f, 228.1f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{536.4f, 5.5f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{664.6f, 208.5f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{853.8f, 231.9f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{927.2f, 41.7f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{1116.2f, 283.1f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{1265.6f, 351.1f}, {8.0f, 8.0f}, (BODY_TYPES)0});
entities->push({{1278.6f, -50.1f}, {51.0f, 480.0f}, (BODY_TYPES)3});
entities->push({{140.0f, 260.4f}, {229.0f, 11.0f}, (BODY_TYPES)3});
entities->push({{188.6f, 195.4f}, {146.0f, 8.0f}, (BODY_TYPES)3});
entities->push({{313.9f, -19.1f}, {20.0f, 15.0f}, (BODY_TYPES)0});
entities->push({{636.1f, 300.1f}, {16.0f, 14.0f}, (BODY_TYPES)0});
entities->push({{244.1f, 242.5f}, {19.0f, 15.0f}, (BODY_TYPES)0});
entities->push({{998.1f, 270.3f}, {39.0f, 18.0f}, (BODY_TYPES)3});
}

#endif
