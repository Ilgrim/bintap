/* opts.c - getopt_long() helping functions.

   `opts.c' is a part of `bintap' program.

   Author:
   Ivan Ivanovich Tatarinov, <ivan-tat@ya.ru>, 2020.

   This is free and unencumbered software released into the public domain.
   For more information, please refer to <http://unlicense.org> */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "opts.h"

const char *get_opt_prefix (char long_form)
{
    if (long_form)
        return "--";
    else
        return "-";
}

char optval_long_int (char long_form, const char *opt_name, char *str, long *val, long min, long max)
{
    char *endptr;
    long x;

    errno = 0;
    x = strtol (str, &endptr, 0);
    if ((errno == ERANGE && (x == LONG_MAX || x == LONG_MIN))
    ||  (errno != 0 && x == 0))
    {
        fprintf (stderr, "Bad numeric format `%s' for option `%s%s'!\n",
            str, get_opt_prefix (long_form), opt_name);
        return 1;
    }
    if (endptr == str)
    {
        fprintf (stderr, "No digits were found in argument `%s' for option `%s%s'!\n",
            str, get_opt_prefix (long_form), opt_name);
        return 1;
    }
    if (*endptr != '\0')
    {
        fprintf (stderr, "Extra characters after a number found in argument `%s' for option `%s%s'!\n",
            str, get_opt_prefix (long_form), opt_name);
        return 1;
    }
    if (x < min || x > max)
    {
        fprintf (stderr, "Numeric value `%s' for option `%s%s' is out of range! Valid range is [%ld; %ld].\n",
            str, get_opt_prefix (long_form), opt_name, min, max);
        return 1;
    }
    *val = x;
    return 0;
}

char optval_char (char long_form, const char *opt_name, char *str, char *val, int min, int max)
{
    long x;

    if (optval_long_int (long_form, opt_name, str, &x, min, max))
        return 1;
    *val = x;
    return 0;
}

char optval_uint (char long_form, const char *opt_name, char *str, unsigned int *val, unsigned int min, unsigned int max)
{
    long x;

    if (optval_long_int (long_form, opt_name, str, &x, min, max))
        return 1;
    *val = x;
    return 0;
}

void scan_opts (const struct ext_option_t *ext_options,
    char store, char *shortopts, int *shorts_size, struct option *longopts, int *longs_size)
{
    int short_i = 0, long_i = 0;
    const struct ext_option_t *opt;

    opt = ext_options;
    while (opt->short_name || opt->long_name)
    {
        if (opt->short_name)
        {
            if (store)
                shortopts[short_i] = opt->short_name;
            short_i++;
            if (opt->has_arg == required_argument)
            {
                if (store)
                    shortopts[short_i] = ':';
                short_i++;
            }
        }
        if (opt->long_name)
        {
            if (store)
            {
                longopts[long_i].name = opt->long_name;
                longopts[long_i].has_arg = opt->has_arg;
                longopts[long_i].flag = NULL;
                longopts[long_i].val = opt->short_name;
            }
            long_i++;
        }
        opt++;
    }

    if (store)
    {
        /* store end mark */
        if (shortopts)
            shortopts[short_i] = '\0';
        longopts[long_i].name = NULL;
        longopts[long_i].has_arg = 0;
        longopts[long_i].flag = NULL;
        longopts[long_i].val = 0;
    }
    else
    {
        /* include end mark */
        *shorts_size = short_i + 1;
        *longs_size = long_i + 1;
    }
}

char init_opts (const struct ext_option_t *ext_options, char **shortopts, struct option **longopts)
{
    int shorts_size, longs_size;
    char *sopts;
    struct option *lopts;

    /* count options */
    scan_opts (ext_options, 0, NULL, &shorts_size, NULL, &longs_size);

    sopts = malloc (shorts_size);
    if (!sopts)
        return 1;

    lopts = malloc (sizeof (struct option) * longs_size);
    if (!lopts)
    {
        free (sopts);
        return 1;
    }

    /* store options */
    scan_opts (ext_options, 1, sopts, NULL, lopts, NULL);

    *shortopts = sopts;
    *longopts = lopts;
    return 0;
}

int find_short_opt (const struct ext_option_t *ext_options, char name)
{
    const struct ext_option_t *opt = ext_options;
    int i;

    for (i = 0; opt->short_name || opt->long_name; opt++, i++)
        if (opt->short_name == name)
            return i;

    return -1;
}

int find_long_opt (const struct ext_option_t *ext_options, const char *name)
{
    const struct ext_option_t *opt = ext_options;
    int i;

    for (i = 0; opt->short_name || opt->long_name; opt++, i++)
        if (opt->long_name == name)
            return i;

    return -1;
}

int set_opt (const struct ext_option_t *ext_options, char long_form, const char *name, int ind)
{
    const struct ext_option_t *opt = &(ext_options[ind]);
    struct setopt_param_t param;

    if (opt->proc)
    {
        param.long_form = long_form;
        param.name = name;
        param.var = opt->var;
        param.flag = opt->flag;
        return opt->proc (&param);
    }
    else
    {
        if (opt->var)
            *((int *) opt->var) = opt->flag;
        return 0;
    }
}

void free_opts (char **shortopts, struct option **longopts)
{
    if (*shortopts)
    {
        free (*shortopts);
        *shortopts = NULL;
    }
    if (*longopts)
    {
        free (*longopts);
        *longopts = NULL;
    }
}
