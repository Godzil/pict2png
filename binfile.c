/**
 *  BinFile - Binary file access ablstrations
 *  binfile.c
 *
 *  Created by Manoël Trapier on 05/04/2024
 *  Copyright (c) 2024 986-Studio.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/fcntl.h>
#include <sys/stat.h>

#include <unistd.h>


#include <log.h>
#include <binfile.h>

static void *load_file(char *filename, uint32_t *fileSize)
{
    int fd;
    void *RetPtr;
    struct stat FileStat;

    fd = open(filename, O_RDONLY);

    fstat(fd, &FileStat);

    RetPtr = mmap(NULL, FileStat.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    *fileSize = FileStat.st_size;

    close(fd);

    if (RetPtr == MAP_FAILED)
    {
        RetPtr = NULL;
    }

    return RetPtr;
}

static int8_t *file_read(binfile_t *binfile, uint32_t s)
{
    int8_t *ret = NULL;

    if ((binfile->pos + s) <= binfile->file_size)
    {
        ret = binfile->data + binfile->pos;
        binfile->pos += s;
    }

    return ret;
}

static void file_seek(binfile_t *binfile, int32_t offset)
{
    binfile->pos += offset;
    if (binfile->pos < 0)
    {
        binfile->pos = 0;
    }

    if (binfile->pos > binfile->file_size)
    {
       binfile->pos = binfile->file_size;
    }
}

binfile_t *file_open(char *filepath)
{
    binfile_t *ret = calloc(sizeof(binfile_t), 1);

    if (ret != NULL)
    {
        ret->data = load_file(filepath, &(ret->file_size));
        if (ret->data)
        {
            ret->pos = 0;
            ret->read = file_read;
            ret->seek = file_seek;
        }
        else
        {
            free(ret);
            ret = NULL;
        }
    }

    return ret;
}

void file_close(binfile_t **binfile)
{
    munmap((*binfile)->data, (*binfile)->file_size);
    memset(*binfile, 0, sizeof(binfile_t));
    *binfile = NULL;
}