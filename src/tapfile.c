/* tapfile.c - simple `.tap' tape file generator.

   `tapfile.c' is a part of `bintap' program.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#include <string.h>
#include "tapfile.h"

void fill_tape_header_name (char *dest, char *src)
{
    char copy = 1;
    int i;

    for (i = 0; i < TAP_HEADER_NAME_LEN; i++)
    {
        if (copy && src[i] == '\0')
            copy = 0;
        if (copy)
            dest[i] = src[i];
        else
            dest[i] = ' ';
    }
}

void tap_start (TAPFILE *self, char *data)
{
    self->data = data;
    self->size = 0;
    self->block_size = 0;
}

void tap_new_block (TAPFILE *self)
{
    if (self->block_size)
        tap_end_block (self);
}

char *tap_get_cur_ptr (TAPFILE *self)
{
    return self->data + self->size + 2 + self->block_size;
}

void tap_put_char (TAPFILE *self, char c)
{
    self->data[self->size + 2 + self->block_size++] = c;
}

void tap_put_data (TAPFILE *self, char *src, unsigned int len)
{
    memcpy (self->data + self->size + 2 + self->block_size, src, len);
    self->block_size += len;
}

void tap_skip_data (TAPFILE *self, unsigned int len)
{
    self->block_size += len;
}

void tap_put_program_header (TAPFILE *self, char *name,
    unsigned int length, unsigned int start_line, unsigned int prog_len)
{
    struct tap_block_header_t *h;
    h = (struct tap_block_header_t *) tap_get_cur_ptr (self);
    h->type = TAP_HDR_PROGRAM;
    fill_tape_header_name (h->name, name);
    h->length = length;
    h->param1 = start_line;
    h->param2 = prog_len;
    tap_skip_data (self, sizeof (struct tap_block_header_t));
}

void tap_put_bytes_header (TAPFILE *self, char *name,
    unsigned int length, unsigned int load_addr, unsigned int extra_addr)
{
    struct tap_block_header_t *h;
    h = (struct tap_block_header_t *) tap_get_cur_ptr (self);
    h->type = TAP_HDR_BYTES;
    fill_tape_header_name (h->name, name);
    h->length = length;
    h->param1 = load_addr;
    h->param2 = extra_addr;
    tap_skip_data (self, sizeof (struct tap_block_header_t));
}

void tap_end_block (TAPFILE *self)
{
    char *data = self->data + self->size + 2;
    unsigned int checksum = 0;
    unsigned int len = self->block_size;

    while (len--)
        checksum ^= *(data++);
    tap_put_char (self, checksum);
    self->data[self->size++] = self->block_size % 256;
    self->data[self->size++] = self->block_size / 256;
    self->size += self->block_size;
    self->block_size = 0;
}

void tap_end (TAPFILE *self)
{
    if (self->block_size)
        tap_end_block (self);
}

unsigned int tap_get_size (TAPFILE *self)
{
    return self->size;
}
