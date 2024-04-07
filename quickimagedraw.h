/**
 *  QuickImageDraw - Image utilities
 *  imagelib.h
 *
 *  Created by Manoël Trapier on 07/04/2024
 *  Copyright (c) 2024 986-Studio.
 */
#ifndef IMAGELIB_H
#define IMAGELIB_H

#include <stdint.h>

#pragma pack(1)
typedef struct Point
{
    int16_t x;
    int16_t y;
} Point;

typedef union Rect
{
    struct
    {
        int16_t top;
        int16_t left;
        int16_t bottom;
        int16_t right;
    } sides;
    struct
    {
        Point topLeft;
        Point botRight;
    } corners;
} Rect;

typedef struct Region
{
    int16_t regionSize;
    Rect rect;
    int8_t more[];
} Region;

typedef struct Bitmap
{
    int16_t rowByte;
    Rect bounds;
    Rect srcRect;
    Rect dstRect;
    int16_t mode;
    int8_t data[];
} Bitmap;
#pragma pack()

typedef struct image_t
{
    
} image_t;

image_t *image_create(Rect picFrame);
void image_drestoy(image_t **image);
void image_save(image_t *image, char *filepath);

void image_setclipregion(image_t *image, Region clipRegion);
void image_blitbit(image_t *image, int8_t bitmap, int mode, Rect bounds, Rect srcBound, Rect dstBound);

#endif /* IMAGELIB_H */