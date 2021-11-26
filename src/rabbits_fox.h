#ifndef RABBITS_FOX_H
#define RABBITS_FOX_H
#include "error.h"

typedef struct ecosystem rf_ecosystem_t;
// typedef enum ecosystem_object rf_ecosystem_object;
typedef enum { RF_EMPTY=' ', RF_RABBIT='C', RF_FOX='R', RF_ROCK='*' } rf_ecosystem_object_t;

rf_ecosystem_t *rf_init_ecosystem(int GEN_PROC_COELHOS, int GEN_PROC_RAPOSAS,
                                  int GEN_COMIDA_RAPOSAS, int N_GEN, int L,
                                  int C, int N);
void rf_insert_object_ecosystem(rf_ecosystem_t *es, rf_ecosystem_object_t obj,
                                int x, int y);

void rf_free_ecosystem(rf_ecosystem_t *es);

void rf_print_ecosystem_environment(rf_ecosystem_t* es);

//int rf_ecosystem_get_N(rf_ecosystem_t *es);
#endif
