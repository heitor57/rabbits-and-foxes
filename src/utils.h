#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include <stdio.h>
#include "rabbits_fox.h"
int file_num_lines(FILE *f);
rf_ecosystem_t *initial_setup(int argc, char **argv);
void print_ecosystem_full_report(rf_ecosystem_t *es) ;
#endif
