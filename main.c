#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <arpa/inet.h>

#include <log.h>
#include <binfile.h>
#include <quickimagedraw.h>

#pragma pack(1)
typedef struct pict_header
{
    int16_t size;
    Rect picFrame;
    int8_t imageData[];
} pict_header;
#pragma pack()

void rect_to_host(Rect *rect)
{
    rect->sides.top = ntohs(rect->sides.top);
    rect->sides.left = ntohs(rect->sides.left);
    rect->sides.bottom = ntohs(rect->sides.bottom);
    rect->sides.right = ntohs(rect->sides.right);
}

void region_to_host(Region *region)
{
    region->regionSize = ntohs(region->regionSize);
    rect_to_host(&region->rect);
}

void bitmap_to_host(Bitmap *bitmap)
{
    bitmap->rowByte = ntohs(bitmap->rowByte);
    rect_to_host(&bitmap->bounds);
    rect_to_host(&bitmap->srcRect);
    rect_to_host(&bitmap->dstRect);
    bitmap->mode = ntohs(bitmap->mode);
}


void header_to_host(pict_header *data)
{
    data->size = htons(data->size);
    rect_to_host(&data->picFrame);
}

void UnpackBits(int8_t **srcPtr, int8_t **dstPtr, uint16_t outSize)
{
    int i;
    int8_t *sPtr = *srcPtr;
    int8_t *dPtr = *dstPtr;

    int8_t code;
    uint16_t byteDone = 0;

    while (byteDone < outSize)
    {
        code = *(sPtr++);

        if (code == -128)
        {
            Log(TLOG_VERBOSE, "UnpackBits", "Got a -128. Ignoring");
        }
        else if (code < 0)
        {
            for (i = 0 ; i < (1 - code) ; i++)
            {
                *(dPtr++) = *sPtr;
                byteDone++;
            }
            sPtr++;
        }
        else
        {
            for (i = 0 ; i < (1 + code) ; i++)
            {
                *(dPtr++) = *(sPtr++);
                byteDone++;
            }
        }
    }
    *dstPtr = dPtr;
    *srcPtr = sPtr;
    Log(TLOG_DEBUG, "UnpackBits", "Asked %d, unpacked %d", outSize, byteDone);
}

typedef void (*opcode_func)(binfile_t *binfile, image_t *image);
#define OPCODE(s) static inline void opcode_##s (binfile_t *binfile, image_t *image)

OPCODE(nop)
{
    Log(TLOG_VERBOSE, "Opcode", "NOP");
}

OPCODE(version)
{
    Log(TLOG_VERBOSE, "Opcode", "Version");
    uint8_t version = *binfile->read(binfile, 1);
    Log(TLOG_VERBOSE, "Version", "Version %d", version);
}

OPCODE(shortcomment)
{
    Log(TLOG_VERBOSE, "Opcode", "ShortComment");
    uint16_t comment = *binfile->read(binfile, 2);
    Log(TLOG_VERBOSE, "Comment", "kind: %d", ntohs(comment));
}

OPCODE(clipregion)
{
    Region *region;
    Log(TLOG_VERBOSE, "Opcode", "ClipRegion");
    region = (Region *)binfile->read(binfile, 2);
    region_to_host(region);
    binfile->seek(binfile, region->regionSize - 2);
    Log(TLOG_VERBOSE, "cliprgn", "  size: %d", region->regionSize);
    Log(TLOG_VERBOSE, "cliprgn", "   top: %d", region->rect.sides.top);
    Log(TLOG_VERBOSE, "cliprgn", "  left: %d", region->rect.sides.left);
    Log(TLOG_VERBOSE, "cliprgn", "bottom: %d", region->rect.sides.bottom);
    Log(TLOG_VERBOSE, "cliprgn", " right: %d", region->rect.sides.right);
    image_setclipregion(image, region);
}
// 184 * 216
OPCODE(packbitsrect)
{
    int i, j, k;
    uint8_t byteCount;
    uint16_t width, height;
    int8_t *bmp;
    int8_t *src, *dest;
    Log(TLOG_VERBOSE, "Opcode", "PackBitsRect");
    Bitmap *bitmap = (Bitmap *)binfile->read(binfile, sizeof(Bitmap));
    bitmap_to_host(bitmap);
    Log(TLOG_VERBOSE, "packbits", "rowBytes: %d", bitmap->rowByte);
    Log(TLOG_VERBOSE, "packbits", "bounds.t: %d", bitmap->bounds.sides.top);
    Log(TLOG_VERBOSE, "packbits", "bounds.l: %d", bitmap->bounds.sides.left);
    Log(TLOG_VERBOSE, "packbits", "bounds.b: %d", bitmap->bounds.sides.bottom);
    Log(TLOG_VERBOSE, "packbits", "bounds.r: %d", bitmap->bounds.sides.right);
    Log(TLOG_VERBOSE, "packbits", "srcRct.t: %d", bitmap->srcRect.sides.top);
    Log(TLOG_VERBOSE, "packbits", "srcRct.l: %d", bitmap->srcRect.sides.left);
    Log(TLOG_VERBOSE, "packbits", "srcRct.b: %d", bitmap->srcRect.sides.bottom);
    Log(TLOG_VERBOSE, "packbits", "srcRct.r: %d", bitmap->srcRect.sides.right);
    Log(TLOG_VERBOSE, "packbits", "dstRct.t: %d", bitmap->dstRect.sides.top);
    Log(TLOG_VERBOSE, "packbits", "dstRct.l: %d", bitmap->dstRect.sides.left);
    Log(TLOG_VERBOSE, "packbits", "dstRct.b: %d", bitmap->dstRect.sides.bottom);
    Log(TLOG_VERBOSE, "packbits", "dstRct.r: %d", bitmap->dstRect.sides.right);
    Log(TLOG_VERBOSE, "packbits", "    mode: %d", bitmap->mode);

    width = getRectWidth(bitmap->bounds);
    if (bitmap->rowByte * 8 > width)
    {
        /* width is too small, just update it */
        width = bitmap->rowByte * 8;
        bitmap->bounds.sides.right = bitmap->bounds.sides.left + width;
    }
    height = getRectHeight(bitmap->bounds);

    bmp = calloc(sizeof(uint8_t), width * height / 8);

    Log(TLOG_VERBOSE, "packbits", " size: %dx%d", width, height);

    src = bitmap->data;
    dest = bmp;

    for (i = 0; i < height; i++)
    {
        byteCount = (uint8_t)(*src++);
        binfile->seek(binfile, byteCount + 1);
        UnpackBits(&src, &dest, bitmap->rowByte);
    }

    image_blitbit(image, bmp, bitmap->mode, &bitmap->bounds,
                  &bitmap->srcRect, &bitmap->dstRect);

    free(bmp);
}

opcode_func opcodes[255] =
{
    [0x00] = opcode_nop,
    [0x01] = opcode_clipregion,

    [0x11] = opcode_version,

    [0x98] = opcode_packbitsrect,

    [0xA0] = opcode_shortcomment,
};

void help()
{
    printf("pict2png -i input -o output\n");
}

void run_opcodes(binfile_t *binfile, image_t *image)
{
    uint8_t opcode = *binfile->read(binfile, 1);
    while(opcode != 0xFF)
    {
        if (opcodes[opcode])
        {
            opcodes[opcode](binfile, image);
        }
        else
        {
            Log(TLOG_ERROR, "Opcodes", "Unknown opcode: 0x%X", opcode);
        }
        opcode = *binfile->read(binfile, 1);
    }
}

int main(int argc, char *argv[])
{
    int param_i;
    char *input_file = NULL;
    char *output_file = NULL;
    image_t *outImage;

    binfile_t *fileContent;
    pict_header *header;
    for (param_i = 1 ; (param_i < argc) && (argv[param_i][0] == '-') ; param_i++)
    {
        switch (argv[param_i][1])
        {
        default: case 'h': help(); exit(-1);
        case 'i': input_file = argv[++param_i]; break;
        case 'o': output_file = argv[++param_i]; break;
        case '-': goto no_more_params;
        }
    }
no_more_params:

    if ((!input_file) || (!output_file))
    {
        help();
        exit(-1);
    }

    Log(TLOG_DEBUG, "params", "Input file: %s", input_file);
    Log(TLOG_DEBUG, "params", "Output file: %s", output_file);

    fileContent = file_open(input_file);
    if ( (fileContent->data[0xA] == 0x11) && (fileContent->data[0xB] == 0x01) )
    {
        Log(TLOG_DEBUG, "loading", "Headerless file", input_file);
    }
    else if ( (fileContent->data[512 + 0xA] == 0x11) && (fileContent->data[512 + 0xB] == 0x01) )
    {
        Log(TLOG_DEBUG, "loading", "File with header, skiping 512 bytes", input_file);
        fileContent->seek(fileContent, 512);
    }
    else
    {
        Log(TLOG_ERROR,"loading", "Either this file is not a PICT file, or it is a PICT v2 file which is not supported");
        return -1;
    }

    header = (pict_header *)fileContent->read(fileContent, 10);

    header_to_host(header);

    Log(TLOG_VERBOSE, "header", "File size: %d", header->size);
    Log(TLOG_VERBOSE, "header", "      top: %d", header->picFrame.sides.top);
    Log(TLOG_VERBOSE, "header", "     left: %d", header->picFrame.sides.left);
    Log(TLOG_VERBOSE, "header", "   bottom: %d", header->picFrame.sides.bottom);
    Log(TLOG_VERBOSE, "header", "    right: %d", header->picFrame.sides.right);

    outImage = image_create(header->picFrame);

    run_opcodes(fileContent, outImage);

    image_save(outImage, output_file);
    image_destroy(&outImage);
    file_close(&fileContent);

    return 0;
}