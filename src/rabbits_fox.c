#include "rabbits_fox.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define next_cell(g, x, y, p) (((g) + (x) + (y)) % (p))


struct ecosystem {
  int GEN_PROC_COELHOS, GEN_PROC_RAPOSAS, GEN_COMIDA_RAPOSAS, N_GEN, L, C, N;
  int current_generation;
  rf_ecosystem_object_t **environment;
};

/*typedef struct {*/
  /*int procreation_age, x, y;*/
/*} rabbit_t;*/
/*typedef struct {*/
  /*int procreation_age, food_generations, x, y;*/
/*} fox_t;*/


/*typedef struct {*/
/*int x, y;*/
/*} coordinate_2d_t;*/

typedef enum { NORTH, EAST, SOUTH, WEST } directions_t;

/*rf_ecosystem_object_t * rf_init_ecosystem_object(rf_ecosystem_object_type_t eco_type_obj){*/

  /*rf_ecosystem_object_t *eco_obj = malloc(sizeof(rf_ecosystem_object_t));*/
  /*eco_obj->type=eco_type_obj;*/
  /*eco_obj->food_generations=0;*/
  /*eco_obj->procreation_age=0;*/
  /*return eco_obj;*/
/*}*/

rf_ecosystem_t *rf_new_ecosystem(int GEN_PROC_COELHOS, int GEN_PROC_RAPOSAS,
                                  int GEN_COMIDA_RAPOSAS, int N_GEN, int L,
                                  int C, int N) {

  rf_ecosystem_t *es = malloc(sizeof(rf_ecosystem_t));

  es->GEN_PROC_COELHOS = GEN_PROC_COELHOS;
  es->GEN_PROC_RAPOSAS = GEN_PROC_RAPOSAS;
  es->GEN_COMIDA_RAPOSAS = GEN_COMIDA_RAPOSAS;
  es->N_GEN = N_GEN;
  es->L = L;
  es->C = C;
  es->N = N;
  es->current_generation = 0;

  es->environment = malloc(sizeof(rf_ecosystem_object_t *) * es->L);
  for (int i = 0; i < es->L; i++) {
    es->environment[i] = malloc(sizeof(rf_ecosystem_object_t) * es->C);
    for (int j = 0; j < es->C; j++) {
      es->environment[i][j].type = RF_EMPTY;
    }
  }
  return es;
}

rf_ecosystem_t *rf_clone_ecosystem(rf_ecosystem_t *es) {
  rf_ecosystem_t *new_es = malloc(sizeof(rf_ecosystem_t));
  *new_es = *es;

  new_es->environment = malloc(sizeof(rf_ecosystem_object_t *) * new_es->L);
  for (int i = 0; i < es->L; i++) {
    new_es->environment[i] = malloc(sizeof(rf_ecosystem_object_t) * new_es->C);

    memcpy(new_es->environment[i], es->environment[i],
           new_es->C * sizeof(rf_ecosystem_object_t));
  }
  return new_es;
}
void rf_clear_environment(rf_ecosystem_t *es) {
  int i, j;
  for (i = 0; i < es->L; i++) {
    for (j = 0; j < es->C; j++) {
      switch (es->environment[i][j].type) {
      case RF_EMPTY:
      case RF_ROCK:
        break;
      default:
        es->environment[i][j].type = RF_EMPTY;
        break;
      }
    }
  }
}
void rf_insert_object_ecosystem(rf_ecosystem_t *es, rf_ecosystem_object_t obj,
                                int x, int y) {
  es->environment[x][y] = obj;
}

void rf_free_ecosystem(rf_ecosystem_t *es) {
  if (es != NULL) {
    if (es->environment != NULL) {
      for (int i = 0; i < es->L; i++) {
        free(es->environment[i]);
      }
      free(es->environment);
    }
    free(es);
  }
}

void rf_print_ecosystem_environment(rf_ecosystem_t *es) {
  int i = 0;
  int j = 0;

  for (j = 0; j < es->C + 2; j++) {
    printf("-");
  }
  printf("\n");
  for (i = 0; i < es->L; i++) {
    printf("|");
    for (j = 0; j < es->C; j++) {
      printf("%c", es->environment[i][j].type);
    }
    printf("|\n");
  }
  for (j = 0; j < es->C + 2; j++) {
    printf("-");
  }
  printf("\n");
}
/*int rf_ecosystem_get_N(rf_ecosystem_t *es){*/
/*return es->N;*/
/*}*/

/*coordinate_2d_t rf_rabbit_new_position(rf_ecosystem_t *es) {*/
/*}*/
void rf_update_ecosystem_rabbits(rf_ecosystem_t *es, rf_ecosystem_t *new_es) {
  int x, y;
  for (x = 0; x < es->L; x++) {
    for (y = 0; y < es->C; y++) {
      if (es->environment[x][y].type == RF_RABBIT) {

        /*es->environment[x][y];*/
        /*es->environment[x + 1][y] es->environment[x][y + 1] es*/
            /*->environment[x][y - 1] es->environment[x - 1][y]*/

            /*next_cell(es->current_generation, x, y, 4);*/
      }
    }
  }
}
void rf_update_ecosystem_foxes(rf_ecosystem_t *es, rf_ecosystem_t *new_es) {}
void rf_update_ecosystem_generation(rf_ecosystem_t *es,
                                    rf_ecosystem_t *new_es) {
  rf_update_ecosystem_rabbits(es, new_es);
  rf_update_ecosystem_foxes(es, new_es);
  new_es->current_generation++;
  es->current_generation++;
}

rf_ecosystem_t *rf_update_ecosystem_generations(rf_ecosystem_t *es) {
  rf_ecosystem_t *new_es = rf_clone_ecosystem(es);
  rf_ecosystem_t *tmp;
#ifndef NDEBUG
  printf("New ecosystem environment:\n");
  rf_print_ecosystem_environment(new_es);
#endif

  for (int i = 0; i < es->N_GEN; i++) {
    rf_clear_environment(new_es);
    rf_update_ecosystem_generation(es, new_es);
    tmp = es;
    es = new_es;
    new_es = tmp;
  }
  return es;
}
