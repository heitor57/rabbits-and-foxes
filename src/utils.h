#ifndef UTILS_H
#define UTILS_H
#include "rabbits_fox.h"
#include <stdbool.h>
#include <stdio.h>
int file_num_lines(FILE *f);
rf_ecosystem_t *initial_setup();
void print_ecosystem_full_report(rf_ecosystem_t *es);
#endif
