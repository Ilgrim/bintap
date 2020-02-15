/* basic.c - simple BASIC program generator.

   `basic.c' is a part of `bintap' program.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#include <stdio.h>
#include <string.h>
#include "basic.h"

void bas_start (BASPROG *self, char *data, unsigned int start, unsigned int inc)
{
    self->data = data;
    self->size = 0;
    self->line_start = start;
    self->line_inc = inc;
    self->line_num = 0;
    self->line_size = 0;
}

void bas_start_line (BASPROG *self, unsigned int num)
{
    if (self->line_size)
        bas_end_line (self);
    self->line_num = num;
    self->line_size = 0;
}

void bas_new_line (BASPROG *self)
{
    if (self->line_size)
        bas_end_line (self);
    self->line_num = self->line_start;
    self->line_size = 0;
}

void bas_put_char (BASPROG *self, char c)
{
    self->data[self->size + 4 + self->line_size++] = c;
}

void bas_put_ascii (BASPROG *self, char *s)
{
    unsigned int len = strlen (s);

    memcpy (self->data + self->size + 4 + self->line_size, s, len);
    self->line_size += len;
}

/* Valid range for integer is [-65535; 65535] */

void bas_put_int_ascii (BASPROG *self, int i)
{
    char s[7];  /* ASCII "-nnnnn\0" */

    snprintf (s, 7, "%d", i);
    bas_put_ascii (self, s);
}

void bas_put_int_integral (BASPROG *self, int i)
{
    bas_put_char (self, 0x0E);      /* integral number follows */
    bas_put_char (self, 0x00);      /* always zero */
    if (i < 0)
    {
        bas_put_char (self, 0xFF);  /* negative sign flag */
        i += 0x10000;
    }
    else
        bas_put_char (self, 0x00);  /* positive sign flag */
    bas_put_char (self, i % 256);
    bas_put_char (self, i / 256);
    bas_put_char (self, 0x00);      /* always zero */
}

void bas_put_int (BASPROG *self, int i)
{
    bas_put_int_ascii (self, i);
    bas_put_int_integral (self, i);
}

void bas_put_int_compact (BASPROG *self, int i)
{
    /* only those numbers are implemented that
       are relatively fast interpreted by BASIC */
    unsigned int n;

    if (i < 0)
    {
        bas_put_char (self, '-');
        n = -i;
    }
    else
        n = i;
    if (n == 0)
        bas_put_ascii (self, SYM_NOT SYM_PI);
    else if (n == 1)
        bas_put_ascii (self, SYM_SGN SYM_PI);
    else if (n == 3)
        bas_put_ascii (self, SYM_INT SYM_PI);
    else if (n >= 32 && n != '"' && n <= 255)
    {
        bas_put_ascii (self, SYM_CODE "\"");
        bas_put_char (self, n);
        bas_put_char (self, '"');
    }
    else
    {
        bas_put_ascii (self, SYM_VAL "\"");
        bas_put_int_ascii (self, n);
        bas_put_char (self, '"');
    }
}

void bas_put_int_secret (BASPROG *self, int i)
{
    bas_put_char (self, '0');
    bas_put_int_integral (self, i);
}

void bas_end_line (BASPROG *self)
{
    bas_put_char (self, LEX_CR);
    self->data[self->size++] = self->line_num / 256;
    self->data[self->size++] = self->line_num % 256;
    self->data[self->size++] = self->line_size % 256;
    self->data[self->size++] = self->line_size / 256;
    self->size += self->line_size;
    self->line_start = self->line_num + self->line_inc;
}

void bas_end (BASPROG *self)
{
    if (self->line_size)
        bas_end_line (self);
}

unsigned int bas_get_size (BASPROG *self)
{
    return self->size;
}
