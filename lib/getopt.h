#ifndef _getopt_h
#define _getopt_h

#ifndef _MSC_VER
#include <getopt.h>
#else
struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

extern char *optarg;
extern int optind;


int getopt_long(int argc, char *const *argv, const char *shortopts, const struct option *longopts, int *indexptr);
#endif
#endif
