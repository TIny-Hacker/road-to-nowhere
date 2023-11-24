#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct object_t {
    int x;
    int y;
    int8_t velocity;
    int frame;
    uint8_t type;
};

#define TILE_WIDTH          20
#define TILE_HEIGHT         20

#define TILEMAP_DRAW_WIDTH  16
#define TILEMAP_DRAW_HEIGHT 12

#define SECONDS             0
#define MINUTES             1
#define HOURS               2

#define DAYS                0
#define MONTHS              1
#define YEARS               2

#define MAX_CARS            5
#define MAX_CLOUDS          5
#define MAX_STARS           20
#define MAX_SNOW            30

#ifdef __cplusplus
}
#endif

#endif
