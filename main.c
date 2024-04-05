#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <arpa/inet.h>

#include <log.h>
#include <binfile.h>

#include <lodepng.h>

#pragma pack(1)
typedef struct Point
{
    uint16_t x;
    uint16_t y;
} Point;

typedef union Rect
{
    struct
    {
        uint16_t top;
        uint16_t left;
        uint16_t bottom;
        uint16_t right;
    } sides;
    struct
    {
        Point topLeft;
        Point botRight;
    } corners;
} Rect;

typedef struct pict_header
{
    uint16_t size;
    Rect picFrame;
    int8_t imageData[];
} pict_header;
#pragma pack()

typedef void (*opcode_func)();

#ifdef __LITTLE_ENDIAN__
void window_rect_to_host(Rect *rect)
{
    rect->sides.top = htons(rect->sides.top);
    rect->sides.left = htons(rect->sides.left);
    rect->sides.bottom = htons(rect->sides.bottom);
    rect->sides.right = htons(rect->sides.right);
}

void header_to_host(pict_header *data)
{
    data->size = htons(data->size);
    window_rect_to_host(&data->picFrame);
}
#else
void window_rect_to_host(Rect *rect) {}
void header_to_host(pict_header *data) {}
#endif

void help()
{
    printf("pict2png -i input -o output\n");
}

int main(int argc, char *argv[])
{
    int param_i;
    char *input_file = NULL;
    char *output_file = NULL;

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

    printf("%d --- %d\n", DEFAULT_DEBUG_LEVEL, MAXIMUM_DEBUG_LEVEL);

    Log(TLOG_DEBUG, "params", "Input file: %s", input_file);
    Log(TLOG_DEBUG, "params", "Output file: %s", output_file);

    fileContent = file_open(input_file);
    header = (pict_header *)fileContent;

    header_to_host(header);

    Log(TLOG_VERBOSE, "header", "File size: %d", header->size);
    Log(TLOG_VERBOSE, "header", "      top: %d", header->picFrame.sides.top);
    Log(TLOG_VERBOSE, "header", "     left: %d", header->picFrame.sides.left);
    Log(TLOG_VERBOSE, "header", "   bottom: %d", header->picFrame.sides.bottom);
    Log(TLOG_VERBOSE, "header", "    right: %d", header->picFrame.sides.right);


    file_close(&fileContent);

    return 0;
}