/**
 * --------------------------------------
 * 
 * Road to Nowhere
 * By TIny_Hacker
 * Copyright 2023
 * License: GPL-3.0
 * 
 * --------------------------------------
**/

#include "defines.h"

#include "asm/spi.h"
#include "gfx/gfx.h"

#include <graphx.h>
#include <keypadc.h>
#include <sys/util.h>
#include <sys/rtc.h>
#include <time.h>

extern unsigned char background1[];

int main(void) {
    srand(rtc_Time());

    gfx_tilemap_t background;

    background.map = background1;
    background.tiles = tilesDefault_tiles;
    background.type_width = gfx_tile_no_pow2;
    background.type_height = gfx_tile_no_pow2;
    background.tile_width = TILE_WIDTH;
    background.tile_height = TILE_HEIGHT;
    background.draw_width = TILEMAP_DRAW_WIDTH;
    background.draw_height = TILEMAP_DRAW_HEIGHT;
    background.width = TILEMAP_DRAW_WIDTH;
    background.height = TILEMAP_DRAW_HEIGHT;
    background.y_loc = 0;
    background.x_loc = 0;

    uint8_t time[3];
    bool useClouds = true;
    boot_GetDate(NULL, &time[MONTHS], NULL);
    bool snowing = (time[MONTHS] <= 3 || time[MONTHS] >= 11);

    setupSPI();
    gfx_Begin();
    gfx_SetTransparentColor(16);
    gfx_SetDrawBuffer();
    gfx_Tilemap_NoClip(&background, 0, 0);
    gfx_SetDrawScreen();

    while (kb_AnyKey());

    clock_t clockOffset = CLOCKS_PER_SEC / 20 + 1; // Draw first frame

    struct object_t cars[MAX_CARS];
    struct object_t clouds[MAX_CLOUDS];
    struct object_t stars[MAX_STARS];
    struct object_t snow[MAX_SNOW];

    uint8_t numberOfCars = 0;
    uint8_t numberOfClouds = 0;
    uint8_t numberOfStars = 0;
    uint8_t numberOfSnow = 0;

    gfx_sprite_t *cloudSprites[2] = {cloud0, cloud1};
    gfx_sprite_t *vehicleSprites[4] = {car, womptruck, walrii, lettuceDelivery};

    while (!kb_AnyKey()) {
        if (clock() - clockOffset > CLOCKS_PER_SEC / 20) {
            for (uint8_t i = 0; i < numberOfCars - 1; i++) { // Detect crashes first
                for (uint8_t j = i + 1; j < numberOfCars; j++) {
                    if ((cars[i].velocity & ~127) == (cars[j].velocity & ~127)) {
                        if (i != j && gfx_CheckRectangleHotspot(
                            cars[i].x, cars[i].y, vehicleSprites[cars[i].type]->width, vehicleSprites[cars[i].type]->height,
                            cars[j].x, cars[j].y, vehicleSprites[cars[j].type]->width, vehicleSprites[cars[j].type]->height)) {

                            gfx_BlitRectangle(gfx_buffer, cars[i].x, cars[i].y, vehicleSprites[cars[i].type]->width, vehicleSprites[cars[i].type]->height);
                            gfx_BlitRectangle(gfx_buffer, cars[j].x, cars[j].y, vehicleSprites[cars[j].type]->width, vehicleSprites[cars[j].type]->height);
                            gfx_BlitScreen();
                            int explosionX = cars[i].x + vehicleSprites[cars[i].type]->width * (cars[i].x < cars[j].x);;
                            int explosionY = cars[i].y + vehicleSprites[cars[i].type]->height / 2;
                            gfx_sprite_t *tempSprite1 = gfx_MallocSprite(vehicleSprites[cars[i].type]->width, vehicleSprites[cars[i].type]->height);
                            gfx_sprite_t *tempSprite2 = gfx_MallocSprite(vehicleSprites[cars[j].type]->width, vehicleSprites[cars[j].type]->height);

                            if (cars[i].velocity < 0) {
                                gfx_FlipSpriteY(vehicleSprites[cars[i].type], tempSprite1);
                                gfx_FlipSpriteY(vehicleSprites[cars[j].type], tempSprite2);
                            } else {
                                free(tempSprite1);
                                free(tempSprite2);
                                tempSprite1 = vehicleSprites[cars[i].type];
                                tempSprite2 = vehicleSprites[cars[j].type];
                            }

                            gfx_TransparentSprite(tempSprite1, cars[i].x, cars[i].y);
                            gfx_TransparentSprite(tempSprite2, cars[j].x, cars[j].y);

                            for (uint8_t shakeFrame = 0; shakeFrame < 25; shakeFrame++) {
                                gfx_ShiftDown(3);
                                gfx_ShiftUp(3);
                                gfx_ShiftLeft(3);
                                gfx_ShiftRight(3);
                                gfx_SetColor(randInt(8, 10));
                                gfx_FillCircle(explosionX + randInt(-shakeFrame, shakeFrame), explosionY + randInt(-shakeFrame, shakeFrame), randInt(3, 6));
                            }

                            while (cars[i].y + vehicleSprites[cars[i].type]->height > 0 || cars[j].y < 240) {
                                beginFrame();
                                gfx_BlitBuffer();

                                if (cars[i].y + vehicleSprites[cars[i].type]->height > 0) {
                                    cars[i].y -= 20;
                                }

                                if (cars[j].y < 240) {
                                    cars[j].y += 20;
                                }

                                gfx_TransparentSprite(tempSprite1, cars[i].x, cars[i].y);
                                gfx_TransparentSprite(tempSprite2, cars[j].x, cars[j].y);
                                endFrame();
                            }

                            for (uint8_t h = i; h < numberOfSnow - 1; h++) { // Probably getting points docked for excessive for loops and yes this could be better but I'm too lazy and it's more fun to add goofy stuff
                                snow[h].x = snow[h + 1].x;
                                snow[h].y = snow[h + 1].y;
                                snow[h].velocity = snow[h + 1].velocity;
                                snow[h].type = snow[h + 1].type;
                                snow[h].frame = snow[h + 1].frame;
                            }

                            for (uint8_t h = j; h < numberOfSnow - 1; h++) {
                                snow[h].x = snow[h + 1].x;
                                snow[h].y = snow[h + 1].y;
                                snow[h].velocity = snow[h + 1].velocity;
                                snow[h].type = snow[h + 1].type;
                                snow[h].frame = snow[h + 1].frame;
                            }

                            numberOfSnow -= 2;
                            gfx_Tilemap_NoClip(&background, 0, 0);

                            if (cars[i].velocity < 0) {
                                free(tempSprite1);
                                free(tempSprite2);
                            }

                            gfx_BlitScreen();
                        }
                    }
                }
            }

            beginFrame();
            gfx_BlitBuffer();
            // Update timer offset
            clockOffset = clock();

            // Adjust palette based on time of day
            boot_GetTime(&time[SECONDS], &time[MINUTES], &time[HOURS]);

            if (time[HOURS] >= 7 && time[HOURS] <= 19) {
                useClouds = true;

                if (time[MINUTES] <= 30 && time[HOURS] == 7) {
                    gfx_SetPalette(betweenPalette, sizeof_betweenPalette, 0);
                } else {
                    gfx_SetPalette(daytimePalette, sizeof_daytimePalette, 0);
                }
            } else {
                if (time[MINUTES] <= 30 && time[HOURS] == 20) {
                    useClouds = true;
                    gfx_SetPalette(betweenPalette, sizeof_betweenPalette, 0);
                } else {
                    useClouds = false;
                    gfx_SetPalette(nighttimePalette, sizeof_nighttimePalette, 0);
                }
            }

            if (useClouds) {
                if (randInt(1, 100) == 1 && numberOfClouds < MAX_CLOUDS) {
                    bool direction = randInt(0, 1);
                    clouds[numberOfClouds].x = -60 + 380 * direction;
                    clouds[numberOfClouds].y = randInt(0, 28);
                    clouds[numberOfClouds].velocity = 1 - 2 * (direction);
                    clouds[numberOfClouds].type = randInt(0, 1);
                    numberOfClouds++;
                }

                for (uint8_t i = 0; i < numberOfClouds; i++) {
                    clouds[i].x += clouds[i].velocity;

                    if (clouds[i].x > 320 || clouds[i].x < -60) {
                        for (uint8_t j = i; j < numberOfClouds - 1; j++) { // Delete cloud and shift list (probably a better way to do this)
                            clouds[j].x = clouds[j + 1].x;
                            clouds[j].y = clouds[j + 1].y;
                            clouds[j].velocity = clouds[j + 1].velocity;
                            clouds[j].type = clouds[j + 1].type;
                        }

                        numberOfClouds--;
                    }

                    gfx_TransparentSprite(cloudSprites[clouds[i].type], clouds[i].x, clouds[i].y);
                }
            } else {
                if (randInt(1, 30) == 1 && numberOfStars < MAX_STARS) {
                    stars[numberOfStars].x = randInt(0, 319);
                    stars[numberOfStars].y = randInt(0, 62);
                    stars[numberOfStars].frame = 0;
                    stars[numberOfStars].type = randInt(2, 3);
                    numberOfStars++;
                }

                for (uint8_t i = 0; i < numberOfStars; i++) {
                    stars[i].frame++;

                    if (stars[i].frame >= 300) {
                        gfx_SetColor(12);
                        gfx_FillRectangle(stars[i].x, stars[i].y, stars[i].type, stars[i].type);

                        for (uint8_t j = i; j < numberOfStars - 1; j++) {
                            stars[j].x = stars[j + 1].x;
                            stars[j].y = stars[j + 1].y;
                            stars[j].frame = stars[j + 1].frame;
                            stars[j].type = stars[j + 1].type;
                        }

                        numberOfStars--;
                    }

                    gfx_SetColor(7);
                    gfx_FillRectangle(stars[i].x, stars[i].y, stars[i].type, stars[i].type);
                }
            }

            if (randInt(1, 15) == 1 && numberOfCars < MAX_CARS) {
                bool direction = randInt(0, 1);
                cars[numberOfCars].type = randInt(0, 3);
                cars[numberOfCars].x = -vehicleSprites[cars[numberOfCars].type]->width + (320 + vehicleSprites[cars[numberOfCars].type]->width) * direction;
                cars[numberOfCars].y = randInt(220, 225) - vehicleSprites[cars[numberOfCars].type]->height;
                cars[numberOfCars].y -= 50 * direction;
                cars[numberOfCars].velocity = 5 - 10 * (direction);
                bool carCollides = false;

                for (uint8_t i = 0; i < numberOfCars; i++) { // DON'T SPAWN ON TOP OF EACH OTHER (VERY BAD)
                    if ((cars[numberOfCars].velocity & ~127) == (cars[i].velocity & ~127)) {
                        carCollides = gfx_CheckRectangleHotspot(
                            cars[numberOfCars].x, cars[numberOfCars].y,
                            vehicleSprites[cars[numberOfCars].type]->width, vehicleSprites[cars[numberOfCars].type]->height,
                            cars[i].x, cars[i].y, vehicleSprites[cars[i].type]->width, vehicleSprites[cars[i].type]->height);
                    }
                }

                numberOfCars += !carCollides;
            }

            for (int8_t priority = -1; priority < 2; priority += 2) {
                for (uint8_t i = 0; i < numberOfCars; i++) {
                    if ((cars[i].velocity & ~127) == (priority & ~127)) {
                        cars[i].x += cars[i].velocity;

                        if (cars[i].x > 320 || cars[i].x < -vehicleSprites[cars[i].type]->width) {
                            for (uint8_t j = i; j < numberOfCars - 1; j++) {
                                cars[j].x = cars[j + 1].x;
                                cars[j].y = cars[j + 1].y;
                                cars[j].velocity = cars[j + 1].velocity;
                                cars[j].type = cars[j + 1].type;
                            }

                            numberOfCars--;
                        }

                        gfx_SetDrawBuffer();

                        // Chance of speeding up on snow
                        if (gfx_GetPixel(cars[i].x, cars[i].y + vehicleSprites[cars[i].type]->height) == 7 && randInt(1, 15) == 1) {
                            cars[i].velocity *= 2;
                            asm("push hl\n\tld hl, -1\n\tld(hl), 2\n\tpop hl");
                        }

                        gfx_SetDrawScreen();

                        if (cars[i].velocity < 0) {
                            gfx_sprite_t *tempSprite = gfx_MallocSprite(vehicleSprites[cars[i].type]->width, vehicleSprites[cars[i].type]->height);
                            gfx_FlipSpriteY(vehicleSprites[cars[i].type], tempSprite);
                            gfx_TransparentSprite(tempSprite, cars[i].x, cars[i].y);
                            free(tempSprite);
                        } else {
                            gfx_TransparentSprite(vehicleSprites[cars[i].type], cars[i].x, cars[i].y);
                        }
                    }
                }
            }

            if (snowing && randInt(1, 4) == 1 && numberOfSnow < MAX_SNOW) {
                snow[numberOfSnow].type = randInt(1, 2);
                snow[numberOfSnow].x = randInt(0, 319);
                snow[numberOfSnow].y = randInt(0, 64);
                snow[numberOfSnow].velocity = randInt(1, 3);
                snow[numberOfSnow].frame = randInt(snow[numberOfSnow].y + 1, 239); // Use this as the final Y value where the snow lands in this case
                numberOfSnow++;
            }

            for (uint8_t i = 0; i < numberOfSnow; i++) {
                snow[i].y += snow[i].velocity;

                gfx_SetColor(7);
                gfx_FillRectangle(snow[i].x, snow[i].y, snow[i].type, snow[i].type);

                gfx_SetDrawBuffer();

                if (snow[i].y >= snow[i].frame) { 
                    if (gfx_GetPixel(snow[i].x, snow[i].y + 1) != 12) {
                        gfx_FillRectangle(snow[i].x, snow[i].y, snow[i].type, snow[i].type); // Preserve snow to the buffer as it has "landed"

                        for (uint8_t j = i; j < numberOfSnow - 1; j++) {
                            snow[j].x = snow[j + 1].x;
                            snow[j].y = snow[j + 1].y;
                            snow[j].velocity = snow[j + 1].velocity;
                            snow[j].type = snow[j + 1].type;
                            snow[j].frame = snow[j + 1].frame;
                        }

                        numberOfSnow--;
                    } else {
                        snow[i].frame = randInt(snow[i].y + 1, 239);
                    }
                }

                gfx_SetDrawScreen();
            }

            endFrame();
        }
    }

    gfx_End();

    return 0;
}
