/* basic.h - declarations for `basic.c'.

   `basic.c' is a part of `bintap' program.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#ifndef _basic_h
#define _basic_h 1

/* BASIC interpreter */

/* BASIC lexems (numbers) */
#define LEX_CR          0x0D
#define LEX_PI          0xA7
#define LEX_CODE        0xAF
#define LEX_VAL         0xB0
#define LEX_INT         0xBA
#define LEX_SGN         0xBC
#define LEX_USR         0xC0
#define LEX_AT          0xC1
#define LEX_NOT         0xC3
#define LEX_INK         0xD9
#define LEX_PAPER       0xDA
#define LEX_FLASH       0xDB
#define LEX_BRIGHT      0xDC
#define LEX_INVERSE     0xDD
#define LEX_OVER        0xDE
#define LEX_BORDER      0xE7
#define LEX_REM         0xEA
#define LEX_LOAD        0xEF
#define LEX_POKE        0xF4
#define LEX_PRINT       0xF5
#define LEX_RANDOMIZE   0xF9
#define LEX_CLS         0xFB
#define LEX_CLEAR       0xFD

/* BASIC lexems (characters) to form an ASCIZ string */
#define SYM_CR          "\x0D"
#define SYM_PI          "\xA7"
#define SYM_CODE        "\xAF"
#define SYM_VAL         "\xB0"
#define SYM_INT         "\xBA"
#define SYM_SGN         "\xBC"
#define SYM_USR         "\xC0"
#define SYM_AT          "\xC1"
#define SYM_NOT         "\xC3"
#define SYM_INK         "\xD9"
#define SYM_PAPER       "\xDA"
#define SYM_FLASH       "\xDB"
#define SYM_BRIGHT      "\xDC"
#define SYM_INVERSE     "\xDD"
#define SYM_OVER        "\xDE"
#define SYM_BORDER      "\xE7"
#define SYM_REM         "\xEA"
#define SYM_LOAD        "\xEF"
#define SYM_POKE        "\xF4"
#define SYM_PRINT       "\xF5"
#define SYM_RANDOMIZE   "\xF9"
#define SYM_CLS         "\xFB"
#define SYM_CLEAR       "\xFD"

typedef struct
{
    char *data;
    unsigned int size;
    unsigned int line_start;
    unsigned int line_inc;
    unsigned int line_num;
    unsigned int line_size;
} BASPROG;

void bas_start (BASPROG *self, char *data, unsigned int start, unsigned int inc);
void bas_start_line (BASPROG *self, unsigned int num);
void bas_new_line (BASPROG *self);
void bas_put_char (BASPROG *self, char c);
void bas_put_ascii (BASPROG *self, char *s);
void bas_put_int_ascii (BASPROG *self, int i);
void bas_put_int_integral (BASPROG *self, int i);
void bas_put_int (BASPROG *self, int i);
void bas_put_int_compact (BASPROG *self, int i);
void bas_put_int_secret (BASPROG *self, int i);
void bas_end_line (BASPROG *self);
void bas_end (BASPROG *self);
unsigned int bas_get_size (BASPROG *self);

#endif  /* !_basic_h */
