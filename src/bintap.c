/* bintap - binary to `.tap' tape file converter.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>
#include "opts.h"
#include "tapfile.h"
#include "basic.h"

#define PROGRAM_NAME    "bintap"
#define PROGRAM_VERSION "1.0"

#define PROGRAM_DESCRIPTION \
"Binary to `.tap' tape file converter."

#define PROGRAM_LICENSE \
"License: public domain, <http://unlicense.org>\n\
This is free software; you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law."

#define PROGRAM_AUTHORS \
"Author: Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>."

#define PROGRAM_CONTACTS \
"Home page: <https://gitlab.com/ivan-tat/bintap>"

/* Limits */
#define MAX_LOADER_LEN      256
#define MAX_DATA_LEN        49152
#define MAX_LINE            9999
#define MAX_ADDR            65535
#define MAX_COL             7
#define MAX_FILENAME_LEN    255

/* Default values */
#define DEF_FILE_EXT    ".tap"
#define DEF_START_LINE  32768
#define DEF_LOAD_ADDR   32768
#define DEF_EXTRA_ADDR  32768
#define DEF_CLEAR_ADDR  24575
#define DEF_EXEC_ADDR   32768
#define DEF_BORDER_COL  0
#define DEF_PAPER_COL   0
#define DEF_INK_COL     7

/* Internal BASIC loader generator */
#define BAS_LINE_START  10
#define BAS_LINE_INC    10
#define BAS_LINE_RUN    20

/* General options */
/* Flags */
char            opt_program         = 0;
char            opt_append          = 0;
char            opt_auto_name       = 0;
/* Values */
char           *opt_input           = NULL;
char           *opt_output          = NULL;
char           *opt_title           = NULL;
unsigned int    opt_start_line      = DEF_START_LINE;
unsigned int    opt_load_address    = DEF_LOAD_ADDR;
unsigned int    opt_extra_address   = DEF_EXTRA_ADDR;

/* BASIC loader options */
/* Flags */
char            opt_basic           = 0;
char            opt_d80_syntax      = 0;
char            opt_print_headers   = 1;
/* Values */
unsigned int    opt_clear_address   = DEF_CLEAR_ADDR;
unsigned int    opt_exec_address    = DEF_EXEC_ADDR;
char            opt_border_color    = DEF_BORDER_COL;
char            opt_paper_color     = DEF_PAPER_COL;
char            opt_ink_color       = DEF_INK_COL;

#define HELP_HINT "Use `-h' to get help."

void show_version (void)
{
    fprintf (stdout,
        PROGRAM_NAME ", version " PROGRAM_VERSION " (build " __DATE__ ", " __TIME__ ")\n"
        "%s\n\%s\n\%s\n",
        PROGRAM_LICENSE,
        PROGRAM_AUTHORS,
        PROGRAM_CONTACTS);
}

char Y_or_N (char c)
{
    if (c)
        return 'Y';
    else
        return 'N';
}

void show_help (void)
{
    fprintf (stdout,
"\n\
" PROGRAM_NAME " - %s\n\
\n\
Usage: " PROGRAM_NAME " [OPTIONS] INPUT_FILE\n\
\n\
Options:\n\
  -h, --help                            show this help and exit.\n\
      --version                         show version and exit.\n\
  -p, --program                         make `Program' instead of `Bytes' [%c].\n\
  -t TITLE, --title TITLE               set header name for all blocks.\n\
  -s LINE, --start-line LINE            BASIC start line for program [%u].\n\
  -o FILENAME, --output FILENAME        set output filename.\n\
      --auto-name                       make output filename from input [%c].\n\
  -a, --append                          append tape at end of file [%c].\n\
  -l ADDRESS, --load-address ADDRESS    load address of a binary file [%u].\n\
  -x ADDRESS, --extra-address ADDRESS   extra address of a binary file [%u].\n\
\n\
BASIC loader options:\n\
  -b, --basic                           include BASIC loader [%c].\n\
  -d, --d80                             create D80 syntax loader [%c].\n\
  -c ADDRESS, --clear-address ADDRESS   set clear address [%u].\n\
  -e ADDRESS, --exec-address ADDRESS    set code start address [%u].\n\
      --bc COLOR, --border-color COLOR  set border color [%u].\n\
      --pc COLOR, --paper-color COLOR   set paper color [%u].\n\
      --ic COLOR, --ink-color COLOR     set ink color [%u].\n\
      --nph, --no-print-headers         hide header title when loading [%c].\n\
\n\
Maximum supported input file size is %u bytes.\n\
Maximum `TITLE' length is %u.\n\
`LINE' is a number in range [0; %u].\n\
`ADDRESS' is a number in range [0; %u].\n\
`COLOR' is a number in range [0; %u].\n\
All numbers are decimal or hexadecimal (prefixed with `0x' or `0X').\n",
        PROGRAM_DESCRIPTION,
        Y_or_N (opt_program),
        opt_start_line,
        Y_or_N (opt_auto_name),
        Y_or_N (opt_append),
        opt_load_address,
        opt_extra_address,
        Y_or_N (opt_basic),
        Y_or_N (opt_d80_syntax),
        opt_clear_address,
        opt_exec_address,
        opt_border_color,
        opt_paper_color,
        opt_ink_color,
        Y_or_N (!opt_print_headers),
        MAX_DATA_LEN,
        TAP_HEADER_NAME_LEN,
        MAX_LINE,
        MAX_ADDR,
        MAX_COL);
}

int cmd_help (struct setopt_param_t *p)
{
    show_version ();
    show_help ();
    exit (EXIT_SUCCESS);
}

int cmd_version (struct setopt_param_t *p)
{
    show_version ();
    exit (EXIT_SUCCESS);
}

int setopt_char (struct setopt_param_t *p)
{
    *((char *) p->var) = p->flag;
    return 0;
}

int setopt_string (struct setopt_param_t *p)
{
    if (optarg)
        *((char **) p->var) = optarg;
    return 0;
}

int setopt_line (struct setopt_param_t *p)
{
    return optval_uint (p->long_form, p->name, optarg, (unsigned int *) p->var, 0, MAX_LINE);
}

int setopt_address (struct setopt_param_t *p)
{
    return optval_uint (p->long_form, p->name, optarg, (unsigned int *) p->var, 0, MAX_ADDR);
}

int setopt_color (struct setopt_param_t *p)
{
    return optval_char (p->long_form, p->name, optarg, (char *) p->var, 0, MAX_COL);
}

const struct ext_option_t ext_options[] =
{
    { 'h',  "help",             no_argument,        cmd_help,           NULL, 0 },
    { 0,    "version",          no_argument,        cmd_version,        NULL, 0 },
    { 'p',  "program",          no_argument,        setopt_char,        &opt_program, 1 },
    { 't',  "title",            required_argument,  setopt_string,      &opt_title, 0 },
    { 's',  "start-line",       required_argument,  setopt_line,        &opt_start_line, 0 },
    { 'o',  "output",           required_argument,  setopt_string,      &opt_output, 0 },
    { 0,    "auto-name",        no_argument,        setopt_char,        &opt_auto_name, 1 },
    { 'a',  "append",           no_argument,        setopt_char,        &opt_append, 1 },
    { 'l',  "load-address",     required_argument,  setopt_address,     &opt_load_address, 0 },
    { 'x',  "extra-address",    required_argument,  setopt_address,     &opt_extra_address, 0 },
    { 'b',  "basic",            no_argument,        setopt_char,        &opt_basic, 1 },
    { 'd',  "d80",              no_argument,        setopt_char,        &opt_d80_syntax, 1 },
    { 'c',  "clear-address",    required_argument,  setopt_address,     &opt_clear_address, 0 },
    { 'e',  "exec-address",     required_argument,  setopt_address,     &opt_exec_address, 0 },
    { 0,    "bc",               required_argument,  setopt_color,       &opt_border_color, 0 },
    { 0,    "border-color",     required_argument,  setopt_color,       &opt_border_color, 0 },
    { 0,    "pc",               required_argument,  setopt_color,       &opt_paper_color, 0 },
    { 0,    "paper-color",      required_argument,  setopt_color,       &opt_paper_color, 0 },
    { 0,    "ic",               required_argument,  setopt_color,       &opt_ink_color, 0 },
    { 0,    "ink-color",        required_argument,  setopt_color,       &opt_ink_color, 0 },
    { 0,    "nph",              no_argument,        setopt_char,        &opt_print_headers, 0 },
    { 0,    "no-print-headers", no_argument,        setopt_char,        &opt_print_headers, 0 },
    { 0, NULL, 0, NULL, 0}   /* end mark */
};

char *shortopts = NULL;
struct option *longopts = NULL;

char auto_output_filename (char *dest, const char *src, unsigned int n, const char *ext)
{
    int len, i, count;

    len = strlen (ext);
    strncpy (dest, src, n - len);
    dest[n - len - 1] = '\0';
    i = strlen (dest) - 1;
    count = 0;
    while ((i > 0) && (count < len))
    {
        if (dest[i] == '.')
        {
            dest[i] = '\0';
            break;
        }
        i--;
        count++;
    }
    strcat (dest, ext);
}

void get_tape_header_name (char *dest, char *src)
{
    char copy = 1;
    int i;

    for (i = 0; i < TAP_HEADER_NAME_LEN && copy; i++)
    {
        if (copy && (src[i] == '.' || src[i] == '\0'))
            copy = 0;
        if (copy)
            dest[i] = src[i];
        else
            dest[i] = '\0';
    }
}

void put_loader (TAPFILE *tape, char *basic_name, char *data_name)
{
    BASPROG p;
    char buf[MAX_LOADER_LEN];
    unsigned int len;

    bas_start (&p, buf, BAS_LINE_START, BAS_LINE_INC);
    bas_new_line (&p);
    bas_put_ascii (&p, SYM_REM "loader by " PROGRAM_NAME "-" PROGRAM_VERSION);
    bas_new_line (&p);
    bas_put_char (&p, LEX_BORDER);
    bas_put_int_compact (&p, opt_border_color);
    bas_put_ascii (&p, ":" SYM_PAPER);
    bas_put_int_compact (&p, opt_paper_color);
    bas_put_ascii (&p, ":" SYM_INK);
    bas_put_int_compact (&p, opt_ink_color);
    bas_put_ascii (&p, ":" SYM_BRIGHT);
    bas_put_int_compact (&p, 0);
    bas_put_ascii (&p, ":" SYM_FLASH);
    bas_put_int_compact (&p, 0);
    bas_put_ascii (&p, ":" SYM_INVERSE);
    bas_put_int_compact (&p, 0);
    bas_put_ascii (&p, ":" SYM_CLS);
    bas_new_line (&p);
    bas_put_char (&p, LEX_CLEAR);
    bas_put_int_compact (&p, opt_clear_address);
    if (!opt_print_headers)
    {
        bas_new_line (&p);
        bas_put_char (&p, LEX_POKE);
        bas_put_int_compact (&p, 23739);
        bas_put_char (&p, ',');
        bas_put_int_compact (&p, 111);
    }
    bas_new_line (&p);
    bas_put_char (&p, LEX_LOAD);
    if (opt_d80_syntax)
        bas_put_char (&p, '*');
    bas_put_char (&p, '"');
    bas_put_ascii (&p, data_name);
    bas_put_ascii (&p, "\"" SYM_CODE);
    bas_new_line (&p);
    bas_put_ascii (&p, SYM_RANDOMIZE SYM_USR);
    bas_put_int_compact (&p, opt_exec_address);
    bas_end (&p);

    len = bas_get_size (&p);

    /* new block */
    tap_new_block (tape);
    tap_put_char (tape, TAP_BLK_HEADER);
    tap_put_program_header (tape, basic_name, len, BAS_LINE_RUN, len);
    tap_end_block (tape);

    /* new block */
    tap_new_block (tape);
    tap_put_char (tape, TAP_BLK_DATA);
    tap_put_data (tape, buf, len);
    tap_end (tape);
}

void shutdown (void)
{
    free_opts (&shortopts, &longopts);
}

int main (int argc, char **argv)
{
    char inv_opt = 0, inv_val = 0;
    int c, i;
    char short_name[2];
    const char *opt_name;
    FILE *fi, *fo;
    char *fi_basename;
    unsigned int fi_size;
    char fo_name[MAX_FILENAME_LEN];
    char title[TAP_HEADER_NAME_LEN + 1];
    char buf[(sizeof (struct tap_block_header_t) + 4) * 2 + MAX_LOADER_LEN + MAX_DATA_LEN];
    TAPFILE tape;

    atexit (shutdown);

    if (init_opts (ext_options, &shortopts, &longopts))
    {
        fprintf (stderr, "Failed to allocate memory!\n");
        return 1;
    }

    if (argc > 1)
    {
        optind = 0; /* reset option index */
        while (!inv_opt && !inv_val)
        {
            /* `shortopts' must be a valid ASCIZ string, not NULL */
            c = getopt_long (argc, argv, shortopts, longopts, &i);
            if (c == -1)    /* end of the options */
                break;
            switch (c)
            {
            case 0:
                /* long option */
                opt_name = longopts[i].name;
                i = find_long_opt (ext_options, opt_name);
                if (i == -1)
                    fprintf (stderr, "getopt_long() returned unknown option `%s'!\n", opt_name);
                else
                    inv_val = set_opt (ext_options, 1, opt_name, i);
                break;
            case '?':
                /* getopt_long() already printed an error message. */
                short_name[0] = optopt;
                short_name[1] = '\0';
                opt_name = short_name;
                inv_opt = 1;
                break;
            default:
                /* short option */
                short_name[0] = c;
                short_name[1] = '\0';
                opt_name = short_name;
                i = find_short_opt (ext_options, c);
                if (i == -1)
                    fprintf (stderr, "getopt_long() returned unknown character `\\x%02hhX'!\n", c);
                else
                    inv_val = set_opt (ext_options, 0, opt_name, i);
            }
        }
        if (inv_opt || inv_val)
            /* error messages already printed. */
            return 1;
        else
        {
            i = argc - optind;
            if (i == 1)
                opt_input = argv[optind];
            else if (i > 1)
            {
                fprintf (stderr, "Specify only one input file!\n");
                return 1;
            }
        }
    }

    free_opts (&shortopts, &longopts);

    /* Check values */
    if (!opt_input)
    {
        fprintf (stderr, "%s %s\n", "No input file specified!", HELP_HINT);
        return 1;
    }
    if (!opt_output && !opt_auto_name)
    {
        fprintf (stderr, "%s %s\n", "No output file specified!", HELP_HINT);
        return 1;
    }

    /* Check input filename `opt_input' and get `title' */
    fi_basename = basename (opt_input);
    if (!strcmp (fi_basename, "/")
    ||  !strcmp (fi_basename, "\\")
    ||  !strcmp (fi_basename, ".")
    ||  !strcmp (fi_basename, ".."))
    {
        fprintf (stderr, "Invalid input file name!\n");
        return 1;
    }
    if (opt_title)
        get_tape_header_name (title, opt_title);
    else
        get_tape_header_name (title, fi_basename);
    title[TAP_HEADER_NAME_LEN] = 0;

    /* Get output filename `fo_name' from `opt_output' or `opt_input' */
    if (opt_output)
    {
        strncpy (fo_name, opt_output, MAX_FILENAME_LEN - 1);
        fo_name[MAX_FILENAME_LEN - 1] = '\0';
    }
    else if (opt_auto_name)
        auto_output_filename (fo_name, opt_input, MAX_FILENAME_LEN - 1, DEF_FILE_EXT);

    fi = fopen (opt_input, "rb");
    if (!fi)
    {
        fprintf (stderr, "Failed to open input file!\n");
        return 1;
    }

    /* Get input file size `fi_size' */
    if (fseek (fi, 0, SEEK_END))
    {
        fprintf (stderr, "Failed to seek in input file!\n");
        return 1;
    }
    fi_size = ftell (fi);
    if (fi_size == 0)
    {
        fprintf (stderr, "Input file is empty!\n");
        return 1;
    }
    if (fi_size > MAX_DATA_LEN)
    {
        fi_size = MAX_DATA_LEN;
        fprintf (stderr, "Warning: Input file's size exceeded %u bytes limit!\n", MAX_DATA_LEN);
    }
    if (fseek (fi, 0, SEEK_SET))
    {
        fprintf (stderr, "Failed to seek in input file!\n");
        return 1;
    }

    if (opt_append)
        fo = fopen (fo_name, "ab+");
    else
        fo = fopen (fo_name, "wb+");
    if (!fo)
    {
        fprintf (stderr, "Failed to open output file!\n");
        return 1;
    }

    /* start tape */
    tap_start (&tape, buf); /* `buf' holds the whole tape file */

    if ((!opt_program) && (opt_basic))
    {
        if (opt_d80_syntax)
            put_loader (&tape, "run", title);
        else
            put_loader (&tape, title, title);
    }

    /* new block */
    tap_new_block (&tape);
    tap_put_char (&tape, TAP_BLK_HEADER);
    if (opt_program)
        tap_put_program_header (&tape, title, fi_size, opt_start_line, fi_size);
    else
        tap_put_bytes_header (&tape, title, fi_size, opt_load_address, opt_extra_address);
    tap_end_block (&tape);

    /* new block */
    tap_new_block (&tape);
    tap_put_char (&tape, TAP_BLK_DATA);
    if (!fread (tap_get_cur_ptr (&tape), 1, fi_size, fi))
    {
        fprintf (stderr, "Failed to read input file!\n");
        return 1;
    }
    tap_skip_data (&tape, fi_size);
    tap_end_block (&tape);

    /* stop tape */
    tap_end (&tape);

    /* save */
    fi_size = fwrite (buf, 1, tap_get_size (&tape), fo);
    if (ferror (fo))
    {
        fprintf (stderr, "Failed to save output file!\n");
        return 1;
    }

    fclose (fi);
    fclose (fo);
    return 0;
}
