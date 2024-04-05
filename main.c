#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#include <unistd.h>

#include <arpa/inet.h>

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

void *load_file(char *filename)
{
    int fd;
    void *RetPtr;
    struct stat FileStat;

    fd = open(filename, O_RDONLY);

    fstat(fd, &FileStat);

    RetPtr = mmap(NULL, FileStat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    close(fd);

    if (RetPtr == MAP_FAILED)
    {
        RetPtr = NULL;
    }

    return RetPtr;
}

void help()
{
    printf("pict2png -i input -o output\n");
}

int main(int argc, char *argv[])
{
    int param_i;
    char *input_file = NULL;
    char *output_file = NULL;

    int8_t *fileContent;
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

    fileContent = load_file(input_file);
    header = (pict_header *)fileContent;

    header_to_host(header);

    printf("File size: %d\n", header->size);



    return 0;
}