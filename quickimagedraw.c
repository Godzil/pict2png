/**
 *  QuickImageDraw - Image utilities
 *  quickimagedraw.c
 *
 *  Created by Manoël Trapier on 07/04/2024
 *  Copyright (c) 2024 986-Studio.
 */
#include <stdlib.h>

#include <lodepng.h>

#include <log.h>
#include <quickimagedraw.h>

static uint32_t get_bitmap_pixel(int8_t *bitmap, Rect bounds, int x, int y)
{
    int stride = bounds.sides.right - bounds.sides.left;
    int bitOffset = 7 - (x % 8);
    int pos = (y * stride) / 8 + (x / 8);

    uint32_t colour = (bitmap[pos] & (1 << bitOffset)) == 0 ? 0xFFFFFFFF : 0xFF000000;

    return colour;
}

static void set_image_pixel(image_t *image, int x, int y, uint32_t colour)
{
    uint32_t *pixmap = image->image;
    int stride = image->frame.sides.right - image->frame.sides.left;
    int pos = (y * stride) + x;

    pixmap[pos] = colour;
}

int getRectWidth(Rect r)
{
    return r.sides.right - r.sides.left;
}

int getRectHeight(Rect r)
{
    return r.sides.bottom - r.sides.top;
}

static void stretch(Rect src, Rect dst, int srcX, int srcY, int *dstX, int *dstY)
{
    int x = srcX - src.sides.left;
    int y = srcY - src.sides.top;

    float xRatio = getRectWidth(dst) / getRectWidth(src);
    float yRatio = getRectHeight(dst) / getRectHeight(src);

    x = x * xRatio;
    y = y * yRatio;

    *dstX = x + dst.sides.left;
    *dstY = y + dst.sides.top;
}

image_t *image_create(Rect picFrame)
{
    uint16_t width, height;
    width = picFrame.sides.right - picFrame.sides.left;
    height = picFrame.sides.bottom - picFrame.sides.top;
    image_t *ret = calloc(1, sizeof(image_t));
    if (ret)
    {
        memcpy(&ret->frame, &picFrame, sizeof(Rect));
        ret->image = calloc(width * height, sizeof(uint32_t));
    }

    return ret;
}

void image_destroy(image_t **image)
{
    if ( (*image)->image )
    {
        free((*image)->image);
    }

    memset(*image, 0, sizeof(image_t));

    free(*image);
    *image = NULL;
}

void image_save(image_t *image, char *filepath)
{
    uint16_t width, height;
    width = image->frame.sides.right - image->frame.sides.left;
    height = image->frame.sides.bottom - image->frame.sides.top;
    lodepng_encode32_file(filepath, (uint8_t *)image->image, width, height);
}

void image_setclipregion(image_t *image, Region *clipRegion)
{

}

void image_blitbit(image_t *image, int8_t *bitmap, int mode, Rect *bounds, Rect *srcBound, Rect *dstBound)
{
    int x, y;

    int srcWidth = srcBound->sides.right - srcBound->sides.left;
    int srcHeight = srcBound->sides.bottom - srcBound->sides.top;
    int dstWidth = dstBound->sides.right - dstBound->sides.left;
    int dstHeight = dstBound->sides.bottom - dstBound->sides.top;

    Rect srcRect = {
        .sides.left = srcBound->sides.left - bounds->sides.left,
        .sides.top = srcBound->sides.top - bounds->sides.top,
        .sides.right = srcBound->sides.left - bounds->sides.left + srcWidth,
        .sides.bottom = srcBound->sides.top - bounds->sides.top + srcHeight
    };

    Rect dstRect = {
        .sides.left = dstBound->sides.left - image->frame.sides.left,
        .sides.top = dstBound->sides.top - image->frame.sides.top,
        .sides.right = dstBound->sides.left - image->frame.sides.left + dstWidth,
        .sides.bottom = dstBound->sides.top - image->frame.sides.top + dstHeight
    };

    Log(TLOG_VERBOSE, "BlitBit", "Src Rect: [%d;%d;%d;%d]", srcRect.sides.left, srcRect.sides.top, srcRect.sides.right, srcRect.sides.bottom);
    Log(TLOG_VERBOSE, "BlitBit", "Dst Rect: [%d;%d;%d;%d]", dstRect.sides.left, dstRect.sides.top, dstRect.sides.right, dstRect.sides.bottom);

    for(y = dstRect.sides.top; y < dstRect.sides.bottom; y++)
    {
        for (x = dstRect.sides.left ; x < dstRect.sides.right ; x++)
        {
            int srcX, srcY;
            uint32_t colour;

            stretch(dstRect, srcRect, x, y, &srcX, &srcY);
            colour = get_bitmap_pixel(bitmap, *bounds, srcX, srcY);
            set_image_pixel(image, x, y, colour);
        }
    }
}
