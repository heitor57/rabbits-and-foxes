#include "rabbits_fox.h"
#include <stdio.h>
#include <stdlib.h>
struct ecosystem {
  int GEN_PROC_COELHOS, GEN_PROC_RAPOSAS, GEN_COMIDA_RAPOSAS, N_GEN, L, C, N;
  rf_ecosystem_object_t **environment;
};

rf_ecosystem_t *rf_init_ecosystem(int GEN_PROC_COELHOS, int GEN_PROC_RAPOSAS,
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

  es->environment = malloc(sizeof(rf_ecosystem_object_t *) * es->L);
  for (int i = 0; i < es->L; i++) {
    es->environment[i] = malloc(sizeof(rf_ecosystem_object_t) * es->C);
    for (int j = 0; j < es->C; j++) {
      es->environment[i][j] = RF_EMPTY;
    }
  }
  return es;
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

void rf_print_ecosystem_environment(rf_ecosystem_t* es){
    int i = 0;
    int j = 0;

    for(j = 0 ; j < es->C ; j++){
      printf("-");
    }
    printf("\n");
    for(i = 0 ; i < es->L ; i++) {
        printf("|");
        for(j = 0 ; j < es->C ; j++){
            printf("%c", es->environment[i][j]);
        }
        printf("|\n");
    }
    for(j = 0 ; j < es->C ; j++){
      printf("-");
    }
}
/*int rf_ecosystem_get_N(rf_ecosystem_t *es){*/
  /*return es->N;*/
/*}*/
