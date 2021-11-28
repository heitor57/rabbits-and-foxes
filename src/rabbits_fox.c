#include "rabbits_fox.h"
#include "errors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIRECTIONS_NUMBER 4
#define next_cell(g, x, y, p) (((g) + (x) + (y)) % (p))

typedef enum { NORTH, EAST, SOUTH, WEST } directions_t;

directions_t directions_order[] = {NORTH, EAST, SOUTH, WEST};

void _direction_adjacent_cell(directions_t direction, int *line, int *column) {
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
    _direction_adjacent_cell(direction_tmp, &line1, &column1);
    rf_ecosystem_object_t *tmp_obj =
        rf_get_ecosystem_object(es, line1, column1, &status);

    if (status == E_SUCCESS && tmp_obj->type == obj_to_search_type) {
      directions_available[z] = *directions_counter;
      (*directions_counter)++;
    }
  }
}

void rf_update_ecosystem_rabbits(rf_ecosystem_t *es,
                                 rf_ecosystem_t *buffer_es) {
  int line, column, z;
  int line1, column1;
  int directions_available[DIRECTIONS_NUMBER];
  int directions_counter;
  int next_cell_value;
  directions_t direction_tmp;
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
              _direction_adjacent_cell(direction_tmp, &line1, &column1);

              if ((next_gen_rabbit.procreation_age - 1) ==
                  es->GEN_PROC_COELHOS) {
                buffer_es
                    ->environment[next_gen_rabbit.previous_line]
                                 [next_gen_rabbit.previous_column]
                    .type = RF_RABBIT;
                buffer_es
                    ->environment[next_gen_rabbit.previous_line]
                                 [next_gen_rabbit.previous_column]
                    .procreation_age = 0;
                buffer_es
                    ->environment[next_gen_rabbit.previous_line]
                                 [next_gen_rabbit.previous_column]
                    .previous_line = line1;
                buffer_es
                    ->environment[next_gen_rabbit.previous_line]
                                 [next_gen_rabbit.previous_column]
                    .previous_column = column1;
                next_gen_rabbit.procreation_age = 0;
              }
              if (buffer_es->environment[line1][column1].type == RF_RABBIT) {

                if (buffer_es->environment[line1][column1].procreation_age <
                    next_gen_rabbit.procreation_age) {
                  buffer_es->environment[line1][column1] = next_gen_rabbit;
                }
              } else {
                buffer_es->environment[line1][column1] = next_gen_rabbit;
              }
              break;
            }
          }
        } else {
          buffer_es->environment[line][column] = next_gen_rabbit;
        }
      }
    }
  }
}

void rf_update_ecosystem_foxes(rf_ecosystem_t *es, rf_ecosystem_t *buffer_es) {

  int line, column, z;
  int line1, column1;
  int directions_available[DIRECTIONS_NUMBER];
  int directions_counter;
  int next_cell_value;
  directions_t direction_tmp;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {

      if (es->environment[line][column].type == RF_FOX) {
        rf_ecosystem_object_t next_gen_fox = es->environment[line][column];
        next_gen_fox.food_generations++;
        next_gen_fox.procreation_age++;
        next_gen_fox.previous_line = line;
        next_gen_fox.previous_column = column;

        rf_enumerate_directions(es, directions_available, &directions_counter,
                                line, column, RF_RABBIT);
        if (directions_counter == 0) {
          rf_enumerate_directions(es, directions_available, &directions_counter,
                                  line, column, RF_EMPTY);
        }

        if (directions_counter > 0) {
          next_cell_value = next_cell(es->current_generation, line, column,
                                      directions_counter);

          for (z = 0; z < DIRECTIONS_NUMBER; z++) {
            if (directions_available[z] == next_cell_value) {
              direction_tmp = directions_order[z];
              line1 = line;
              column1 = column;
              _direction_adjacent_cell(direction_tmp, &line1, &column1);

              bool insert_fox_in_environment = false;
              if (buffer_es->environment[line1][column1].type == RF_RABBIT) {
                next_gen_fox.food_generations = 0;
                /*buffer_es->environment[line1][column1] = next_gen_fox;*/
                insert_fox_in_environment = true;
              }

              /*rf_ecosystem_object_t *rnext_gen_fox =*/
              /*&buffer_es->environment[line1][column1];*/
              if (next_gen_fox.food_generations < es->GEN_COMIDA_RAPOSAS) {
                if (buffer_es->environment[line1][column1].type == RF_EMPTY) {
                  /*buffer_es->environment[line1][column1] = next_gen_fox;*/
                  insert_fox_in_environment = true;
                } else if (buffer_es->environment[line1][column1].type ==
                           RF_FOX) {
                  if (buffer_es->environment[line1][column1].procreation_age <
                      next_gen_fox.procreation_age) { // gets the fox with the
                    // oldest procreation_age
                    /*buffer_es->environment[line1][column1] = next_gen_fox;*/
                    insert_fox_in_environment = true;
                  } else if (buffer_es->environment[line1][column1]
                                 .procreation_age ==
                             next_gen_fox
                                 .procreation_age) { // tie procreation_age
                    if (buffer_es->environment[line1][column1]
                            .food_generations >
                        next_gen_fox.food_generations) { // solves with
                                                         // food_generations
                      /*buffer_es->environment[line1][column1] = next_gen_fox;*/
                      insert_fox_in_environment = true;
                    }
                  }
                }
                /*es->environment[line][column] =
                 * buffer_es->environment[line][column];*/
                if ((next_gen_fox.procreation_age - 1) ==
                    buffer_es->GEN_PROC_RAPOSAS) {
                  /*line1 = next_gen_fox.previous_line;*/
                  /*column1 = next_gen_fox.previous_column;*/
                  buffer_es
                      ->environment[next_gen_fox.previous_line]
                                   [next_gen_fox.previous_column]
                      .type = RF_FOX;
                  buffer_es
                      ->environment[next_gen_fox.previous_line]
                                   [next_gen_fox.previous_column]
                      .procreation_age = 0;
                  buffer_es
                      ->environment[next_gen_fox.previous_line]
                                   [next_gen_fox.previous_column]
                      .food_generations = 0;
                  buffer_es
                      ->environment[next_gen_fox.previous_line]
                                   [next_gen_fox.previous_column]
                      .previous_line = line1;
                  buffer_es
                      ->environment[next_gen_fox.previous_line]
                                   [next_gen_fox.previous_column]
                      .previous_column = column1;
                  next_gen_fox.procreation_age = 0;
                }
                if (insert_fox_in_environment) {
                  buffer_es->environment[line1][column1] = next_gen_fox;
                }
              } else {
                buffer_es->environment[line1][column1].type = RF_EMPTY;
              }
              break;
            }
          }
        } else {
          buffer_es->environment[line][column] = next_gen_fox;
        }
      }
    }
  }
}

void rf_update_ecosystem_rabbits_from_buffer(rf_ecosystem_t *es,
                                             rf_ecosystem_t *buffer_es) {
  int line, column;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_RABBIT) {
        es->environment[line][column].type = RF_EMPTY;
        es->N--;
      }
    }
  }
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (buffer_es->environment[line][column].type == RF_RABBIT) {
        es->environment[line][column] = buffer_es->environment[line][column];
        es->N++;
      }
    }
  }
}
void rf_update_ecosystem_foxes_from_buffer(rf_ecosystem_t *es,
                                           rf_ecosystem_t *buffer_es) {
  int line, column;
  rf_ecosystem_object_t buffer_es_obj_tmp,es_obj_tmp;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_FOX) {
        es->environment[line][column].type = RF_EMPTY;
        es->N--;
      }
    }
  }
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      buffer_es_obj_tmp = buffer_es->environment[line][column];
      es_obj_tmp = es->environment[line][column];
      if (buffer_es_obj_tmp.type == RF_FOX) {
        if (es->environment[line][column].type != RF_EMPTY) {
          es->N--;
        }
        es->environment[line][column] = buffer_es->environment[line][column];
        es->N++;
      }
    }
  }
}
void rf_update_ecosystem_generation(rf_ecosystem_t *es,
                                    rf_ecosystem_t *buffer_es) {

  rf_update_ecosystem_rabbits(es, buffer_es);
  rf_update_ecosystem_rabbits_from_buffer(es, buffer_es);
  rf_update_ecosystem_foxes(es, buffer_es);
  rf_update_ecosystem_foxes_from_buffer(es, buffer_es);
}

rf_ecosystem_t *rf_update_ecosystem_generations(rf_ecosystem_t *es) {
  rf_ecosystem_t *buffer_es = rf_clone_ecosystem(es);
  int i = 0;
  int generations_number = es->N_GEN;

#ifndef NDEBUG
  printf("============= [%d] Generation =============\n", i);
  rf_print_ecosystem_environment(es);
#endif

  for (i = 1; i <= generations_number; i++) {
    es->current_generation = i - 1;
    buffer_es->current_generation = i;

    rf_clear_environment(buffer_es);
    rf_update_ecosystem_generation(es, buffer_es);

#ifndef NDEBUG
    printf("============= [%d] Generation =============\n", i);
    rf_print_ecosystem_environment(es);
#endif
    es->N_GEN--;
  }

  rf_free_ecosystem(buffer_es);
  return es;
}
