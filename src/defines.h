/**
 * --------------------------------------
 * 
 * Road to Nowhere Source Code - defines.h
 * By TIny_Hacker
 * Copyright 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#ifndef DEFINES_H
#define DEFINES_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct object_t {
    int x;
    uint8_t y;
    int8_t xVelocity;
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

#define MAX_CARS            5
#define MAX_CLOUDS          5
#define MAX_STARS           20

#ifdef __cplusplus
}
#endif

#endif
