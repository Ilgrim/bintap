/* opts.h - declarations for `opts.c'.

   `opts.c' is a part of `bintap' program.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#ifndef _opts_h
#define _opts_h 1

/* String value is passed in `optarg' variable (see `getopt.h'). */
struct setopt_param_t
{
    char long_form;
    const char *name;
    void *var;
    int flag;
};

/* Terminate the list of options with an element filled with zeros. */
struct ext_option_t
{
    char short_name;    /* '\0' for none */
    char *long_name;    /* NULL for none */
    char has_arg;       /* passed to `getopt_long()' */
    int (*proc) (struct setopt_param_t *);
    void *var;          /* reference to a modifiable variable or NULL */
    int flag;           /* +`var' are passed as parameters to `setopt_*()' functions */
};
/* If `proc' is 0 and `var' is not 0 then `var' is assumed to be a reference to a
   variable of type `int' and recieves the value stored in `flag' (in the same way
   getopt_long() does it). */

char optval_long_int (char long_form, const char *opt_name, char *str, long *val, long min, long max);
char optval_char (char long_form, const char *opt_name, char *str, char *val, int min, int max);
char optval_uint (char long_form, const char *opt_name, char *str, unsigned int *val, unsigned int min, unsigned int max);

char init_opts (const struct ext_option_t *options, char **shortopts, struct option **longopts);
int find_short_opt (const struct ext_option_t *options, char name);
int find_long_opt (const struct ext_option_t *options, const char *name);
int set_opt (const struct ext_option_t *options, char long_form, const char *name, int ind);
void free_opts (char **shortopts, struct option **longopts);

#endif  /* !_opts_h */
