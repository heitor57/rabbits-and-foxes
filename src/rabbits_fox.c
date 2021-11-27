#include "rabbits_fox.h"
#include "errors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIRECTIONS_NUMBER 4
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

directions_t directions_order[] = {NORTH, EAST, SOUTH, WEST};

int get_direction_index(directions_t direction) {
  switch (direction) {
  case NORTH:
    return 0;
  case EAST:
    return 1;
  case SOUTH:
    return 2;
  case WEST:
    return 3;
  }
}
void direction_adjacent_cell(directions_t direction, int *line, int *column) {
  switch (direction) {
  case NORTH:
    (*line)--;
    break;
  case EAST:
    (*column)++;
    break;
  case SOUTH:
    (*line)++;
    break;
  case WEST:
    (*column)--;
    break;
  }
}

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
  obj.procreation_age = 0;
  obj.food_generations = 0;
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

rf_ecosystem_object_t *rf_get_ecosystem_object(rf_ecosystem_t *es, int x, int y,
                                               error_t *status) {
  if (y < es->C && y >= 0 && x >= 0 && x < es->L) {
    *status = E_SUCCESS;
    return &(es->environment[x][y]);
  }
  *status = E_OUT_OF_BOUNDS;
  return NULL;
}

void rf_enumerate_directions(rf_ecosystem_t *es, int *directions_available,
                             int *directions_counter, int line, int column,
                             rf_ecosystem_object_type_t obj_to_search_type) {
  int z, line1, column1;
  directions_t direction_tmp;
  error_t status;
  *directions_counter = 0;

  for (z = 0; z < DIRECTIONS_NUMBER; z++) {
    directions_available[z] = -1;
    line1 = line;
    column1 = column;
    direction_tmp = directions_order[z];
    direction_adjacent_cell(direction_tmp, &line1, &column1);
    rf_ecosystem_object_t *tmp_obj =
        rf_get_ecosystem_object(es, line1, column1, &status);

    if (status == E_SUCCESS && tmp_obj->type == obj_to_search_type) {
      directions_available[z] = *directions_counter;
      (*directions_counter)++;
    }
  }
}

void rf_update_ecosystem_rabbits(rf_ecosystem_t *es, rf_ecosystem_t *new_es) {
  int line, column, z;
  int line1, column1;
  int directions_available[DIRECTIONS_NUMBER];
  int directions_counter;
  int next_cell_value;
  directions_t direction_tmp;
  error_t status;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_RABBIT) {
        rf_ecosystem_object_t next_gen_rabbit = es->environment[line][column];
        next_gen_rabbit.procreation_age++;
        next_gen_rabbit.previous_column = column;
        next_gen_rabbit.previous_line = line;
        rf_enumerate_directions(es, directions_available, &directions_counter,
                                line, column, RF_EMPTY);

        if (directions_counter > 0) {
          next_cell_value = next_cell(es->current_generation, line, column,
                                      directions_counter);

          for (z = 0; z < DIRECTIONS_NUMBER; z++) {
            if (directions_available[z] == next_cell_value) {
              direction_tmp = directions_order[z];
              line1 = line;
              column1 = column;
              direction_adjacent_cell(direction_tmp, &line1, &column1);

#ifndef NDEBUG
              printf("INFO: [%d] RABBIT, %d %d, %d, %d %d\n",
                     new_es->current_generation, line, column,
                     directions_counter, line1, column1);
#endif

              if (new_es->environment[line1][column1].type == RF_RABBIT) {

                if (new_es->environment[line1][column1].procreation_age <
                    next_gen_rabbit.procreation_age) {
                  new_es->environment[line1][column1] = next_gen_rabbit;
                }
              } else {
                new_es->environment[line1][column1] = next_gen_rabbit;
              }
              break;
            }
          }
        } else {
          new_es->environment[line][column] = next_gen_rabbit;
        }
      }
    }
  }
}

void rf_update_ecosystem_foxes(rf_ecosystem_t *es, rf_ecosystem_t *new_es) {

  int line, column, z;
  int line1, column1;
  int directions_available[DIRECTIONS_NUMBER];
  int directions_counter;
  int next_cell_value;
  directions_t direction_tmp;
  error_t status;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {

      if (es->environment[line][column].type == RF_FOX) {
        if (es->environment[line][column].food_generations !=
            es->GEN_COMIDA_RAPOSAS) {
          rf_ecosystem_object_t next_gen_fox = es->environment[line][column];
          next_gen_fox.procreation_age++;
          next_gen_fox.food_generations++;
          next_gen_fox.previous_line = line;
          next_gen_fox.previous_column = column;

          rf_enumerate_directions(es, directions_available, &directions_counter,
                                  line, column, RF_RABBIT);
#ifndef NDEBUG
          printf("INFO: [%d] FOX, %d %d, %d\n", new_es->current_generation,
                 line, column, directions_counter);
#endif
          if (directions_counter == 0) {
            rf_enumerate_directions(es, directions_available,
                                    &directions_counter, line, column,
                                    RF_EMPTY);
          }

          if (directions_counter > 0) {
            next_cell_value = next_cell(es->current_generation, line, column,
                                        directions_counter);

            for (z = 0; z < DIRECTIONS_NUMBER; z++) {
              if (directions_available[z] == next_cell_value) {
                direction_tmp = directions_order[z];
                line1 = line;
                column1 = column;
                direction_adjacent_cell(direction_tmp, &line1, &column1);

                if (new_es->environment[line1][column1].type == RF_FOX) {
                  if (new_es->environment[line1][column1].procreation_age <
                      next_gen_fox.procreation_age) { // gets the fox with the
                    // oldest procreation_age
                    new_es->environment[line1][column1] = next_gen_fox;
                  } else if (new_es->environment[line1][column1]
                                 .procreation_age ==
                             next_gen_fox
                                 .procreation_age) { // tie procreation_age
                    if (new_es->environment[line1][column1].food_generations >
                        next_gen_fox
                            .food_generations) { // solves with food_generations
                      new_es->environment[line1][column1] = next_gen_fox;
                    }
                  }
                } else if (new_es->environment[line1][column1].type ==
                           RF_RABBIT) {
                  next_gen_fox.food_generations = 0;
                  new_es->environment[line1][column1] = next_gen_fox;
                } else {
                  new_es->environment[line1][column1] = next_gen_fox;
                }
                break;
              }
            }
          } else {
            new_es->environment[line][column] = next_gen_fox;
          }
        }
      }
    }
  }
}

void rf_update_ecosystem_generation(rf_ecosystem_t *es,
                                    rf_ecosystem_t *buffer_es) {

  int line, column;
  rf_update_ecosystem_rabbits(es, buffer_es);
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_RABBIT) {
        es->environment[line][column].type = RF_EMPTY;
      }
    }
  }
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (buffer_es->environment[line][column].type == RF_RABBIT) {
        es->environment[line][column] = buffer_es->environment[line][column];
      }
    }
  }

  /*#ifndef NDEBUG*/
  /*printf("[%d] Rabbit new environment:\n",new_es->current_generation);*/
  /*rf_print_ecosystem_environment(new_es);*/
  /*#endif*/

  rf_update_ecosystem_foxes(es, buffer_es);
  /*#ifndef NDEBUG*/
  /*printf("[%d] Fox new environment:\n"esnew_es->current_generation);*/
  /*rf_print_ecosystem_environment(new_es);*/
  /*#endif*/

  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_FOX) {
        es->environment[line][column].type = RF_EMPTY;
      }
    }
  }
  rf_ecosystem_object_t buffer_es_obj_tmp, es_obj_tmp;
  int line1, column1;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      buffer_es_obj_tmp = buffer_es->environment[line][column];
      es_obj_tmp = es->environment[line][column];
      if (buffer_es_obj_tmp.type == RF_FOX) {
        if (buffer_es_obj_tmp.procreation_age-1 == buffer_es->GEN_PROC_RAPOSAS) {
          line1 = buffer_es_obj_tmp.previous_line;
          column1 = buffer_es_obj_tmp.previous_column;
          es->environment[line1][column1].type = RF_FOX;
          es->environment[line1][column1].procreation_age = 0;
          es->environment[line1][column1].food_generations = 0;
          es->environment[line1][column1].previous_line = line1;
          es->environment[line1][column1].previous_column = column1;
          buffer_es_obj_tmp.procreation_age = 0;
        }
        es->environment[line][column] = buffer_es_obj_tmp;
      } else if (es_obj_tmp.type == RF_RABBIT) {
        if (es_obj_tmp.procreation_age-1 == es->GEN_PROC_COELHOS) {
          line1 = es_obj_tmp.previous_line;
          column1 = es_obj_tmp.previous_column;
          es->environment[line1][column1].type = RF_RABBIT;
          es->environment[line1][column1].procreation_age = 0;
          es->environment[line1][column1].previous_line = line1;
          es->environment[line1][column1].previous_column = column1;
          es_obj_tmp.procreation_age = 0;
        }
      }
    }
  }
}

rf_ecosystem_t *rf_update_ecosystem_generations(rf_ecosystem_t *es) {
  rf_ecosystem_t *buffer_es = rf_clone_ecosystem(es);
  rf_ecosystem_t *tmp;
  /*#ifndef NDEBUG*/
  /*printf("Cloned environment:\n");*/
  /*rf_print_ecosystem_environment(buffer_es);*/
  /*#endif*/

  for (int i = 1; i <= es->N_GEN; i++) {
    es->current_generation = i - 1;
    buffer_es->current_generation = i;

#ifndef NDEBUG
    printf("============= [%d] =============\n", i);
#endif
    rf_clear_environment(buffer_es);
    rf_update_ecosystem_generation(es, buffer_es);

#ifndef NDEBUG
    printf("new ecosystem environment:\n", i);
    rf_print_ecosystem_environment(es);
#endif
    /*tmp = es;*/
    /*es = new_es;*/
    /*new_es = tmp;*/
  }
  return es;
}
