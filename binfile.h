/**
 *  BinFile - Binary file access abstrations
 *  binfile.h
 *
 *  Created by Manoël Trapier on 05/04/2024
 *  Copyright (c) 2024 986-Studio.
 *
 */
#ifndef BINFILE_H
#define BINFILE_H

#include <stdio.h>
#include <stdint.h>

typedef struct binfile_t
{
    int8_t *data;
    int64_t pos;
    uint32_t file_size;

    int8_t *(*read)(struct binfile_t *binfile, uint32_t s);
    void (*seek)(struct binfile_t *binfile, int32_t offset);
} binfile_t;

binfile_t *file_open(char *filepath);
void file_close(binfile_t **binfile);

#endif /* BINFILE_H */