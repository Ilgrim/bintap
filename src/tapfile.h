/* tapfile.h - declarations for `tapfile.c'.

   `tapfile.c' is a part of `bintap' program.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#ifndef _tapfile_h
#define _tapfile_h 1

/* Format of BASIC tape block
   (variable length - at least 4 bytes long with no data at all):

   Offset    Type   Name      Description
   0000-0001 uint16 length    including `type' and `checksum' bytes
   0002      uint8  type      00 = header, FF = data
   0003-nnnn uint8  data[]    `length-2' bytes long (excluding `type' and `checksum')
   nnnn+1    uint8  checksum  calculated for 0002-nnnn bytes */

/* Block type */
#define TAP_BLK_HEADER  0
#define TAP_BLK_DATA    0xFF

/* Format of header (field `data', 17 bytes long):

   Offset    Type   Name      Description
   0000      uint8  type      00 = Program, 03 = Bytes
   0001-000A uint8  name[10]  space padded block's name
   000B-000C uint16 length    Program, Bytes: length of a block
   000D-000E uint16 param1    Program: BASIC start line
                              Bytes: start address
   000F-0010 uint16 param2    Program: BASIC program length
                              (in no variables - equals to `length'),
                              Bytes: unused, equals to 32768. */

/* Standard BASIC tape block's header structure */

/* Header type */
#define TAP_HDR_PROGRAM         0
#define TAP_HDR_NUMBER_ARRAY    1
#define TAP_HDR_CHARACTER_ARRAY 2
#define TAP_HDR_BYTES           3

#define TAP_HEADER_NAME_LEN     10
struct tap_block_header_t
{
    char type;
    char name[TAP_HEADER_NAME_LEN];
    unsigned short length;
    unsigned short param1;
    unsigned short param2;
    /* 17 bytes, must not be aligned */
} __attribute__((aligned(1),packed));

void fill_tape_header_name (char *dest, char *src);

typedef struct
{
    char *data;
    unsigned int size;
    unsigned int block_size;
} TAPFILE;

void tap_start (TAPFILE *self, char *data);
void tap_new_block (TAPFILE *self);
char *tap_get_cur_ptr (TAPFILE *self);
void tap_put_char (TAPFILE *self, char c);
void tap_put_data (TAPFILE *self, char *src, unsigned int len);
void tap_skip_data (TAPFILE *self, unsigned int len);
void tap_put_header (TAPFILE *self, char type, char *name, unsigned int length, unsigned int param1, unsigned int param2);
void tap_put_program_header (TAPFILE *self, char *name,
    unsigned int length, unsigned int start_line, unsigned int vars_off);
void tap_put_bytes_header (TAPFILE *self, char *name,
    unsigned int length, unsigned int load_addr, unsigned int extra_addr);
void tap_end_block (TAPFILE *self);
void tap_end (TAPFILE *self);
unsigned int tap_get_size (TAPFILE *self);

#endif  /* !_tapfile_h */
