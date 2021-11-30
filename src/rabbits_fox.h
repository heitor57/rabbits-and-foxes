#ifndef RABBITS_FOX_H
#define RABBITS_FOX_H
#include "error.h"

// Tipos de objeto do ecossistema.
typedef enum {
  RF_EMPTY = ' ',// tipo vazio
  RF_RABBIT = 'C',// tipo coelho
  RF_FOX = 'R',// tipo raposa
  RF_ROCK = '*'// tipo rocha
} rf_ecosystem_object_type_t;
// Objeto do ecossistema, que pode ser nada, um coelho, uma raposa, ou uma rocha. Ele contém os atributos em relação à fome, reprodução e coordenadas do objeto.
typedef struct {
  int procreation_age, food_generations;
  rf_ecosystem_object_type_t type;
  int previous_line, previous_column;
} rf_ecosystem_object_t;

// Ecossistema que contém os parâmetros para simulação incluindo o próprio ambiente com as raposas, coelhos e rochas.
typedef struct  {
  int GEN_PROC_COELHOS, GEN_PROC_RAPOSAS, GEN_COMIDA_RAPOSAS, N_GEN, L, C, N;
  int current_generation;// geração corrente
  rf_ecosystem_object_t **environment;// ambiente com os objetos
}rf_ecosystem_t;

rf_ecosystem_t *rf_new_ecosystem(int GEN_PROC_COELHOS, int GEN_PROC_RAPOSAS,
                                 int GEN_COMIDA_RAPOSAS, int N_GEN, int L,
                                 int C, int N);
void rf_insert_object_ecosystem(rf_ecosystem_t *es, rf_ecosystem_object_t obj,
                                int x, int y);

void rf_free_ecosystem(rf_ecosystem_t *es);

void rf_print_ecosystem_environment(rf_ecosystem_t *es);

rf_ecosystem_t *rf_update_ecosystem_generations(rf_ecosystem_t *es);
#endif
