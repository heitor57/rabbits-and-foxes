#include "rabbits_fox.h"
#include "errors.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef PARALLEL
#include <omp.h>
// matriz de lock que indica uma região do ambiente que está sendo acessado ou escrito, é utilizado na implementação paralela.
omp_lock_t **LOCK_MATRIX;
#endif
// Contador utilizado em algumas funções, está sendo usado como uma variável global, pois deixa o código mais legível com a implementação paralela e sem modificações só necessárias para a solução paralela.
int COUNTER;
// Número de direções, isto é, norte, leste, sul, oeste
#define DIRECTIONS_NUMBER 4
// Regra para escolha da próxima célula. É usada para o objeto se mover.
#define next_cell(g, x, y, p) (((g) + (x) + (y)) % (p))

// Direções que as raposas e coelhos podem se mover, isto é: norte; leste; sul; e oeste.
typedef enum { NORTH, EAST, SOUTH, WEST } directions_t;

// Ordem das direções no vetor de enumeração de direções disponíveis
const directions_t directions_order[] = {NORTH, EAST, SOUTH, WEST};

/**
 * @brief Obtém a linha e a coluna dado uma linha, coluna e uma direção.
 *
 * @param direction Direção que irá determinar a próxima linha e coluna.
 * @param line Linha base para ser modificada dada a direção escolhida.
 * @param column Coluna base para ser modificada dada a direção escolhida.
 */
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

/**
 * @brief Cria um novo ecossistema dado os parâmetros.
 *
 * @param GEN_PROC_COELHOS Número de gerações até que um coelho possa procriar.
 * @param GEN_PROC_RAPOSAS Número de gerações até que uma raposa
possa procriar.
 * @param GEN_COMIDA_RAPOSAS Número de gerações para uma raposa
morrer de fome.
 * @param N_GEN Número de gerações para a simulação
 * @param L Número de linhas da matriz representando o ecossistema
 * @param C Número de colunas da matriz representando o ecossistema
 * @param N Número de objetos no ecossistema inicial
 * @return Ecosistema criado com base nos parâmetros passado
 */
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

/**
 * @brief Faz um clone de um ecossistema preexistente.
 *
 * @param es Ambiente a ser duplicado.
 * @return Ambiente duplicado.
 */
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
/**
 * @brief Limpa os objetos vivos do ambiente.
 *
 * @param es Ecossistema a ter seu ambiente limpo.
 */
void rf_clear_environment(rf_ecosystem_t *es) {
  int i, j;

#ifdef PARALLEL
#pragma omp for
#endif
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
/**
 * @brief Insere um novo objeto no ecossistema.
 *
 * @param es Ecosistema a ter um objeto inserido.
 * @param obj Objeto a ser inserido.
 * @param x Linha no qual o objeto será inserido.
 * @param y Coluna no qual o objeto será inserido.
 */
void rf_insert_object_ecosystem(rf_ecosystem_t *es, rf_ecosystem_object_t obj,
                                int x, int y) {
  obj.procreation_age = 0;
  obj.food_generations = 0;
  es->environment[x][y] = obj;
}

/**
 * @brief Libera o espaço de memória reservado para o ecossistema.
 *
 * @param es Ecossistema a ter a memória liberada.
 */
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

/**
 * @brief Imprime o ambiente (matriz) do ecossistema com os objetos.
 *
 * @param es Ambiente a ter o ambiente imprimido.
 */
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

/**
 * @brief Obtém um objeto do ambiente.
 *
 * @param es Ambiente a ter o objeto obtido.
 * @param x Linha do objeto a ser obtido.
 * @param y Coluna do objeto a ser obtido.
 * @param status Status da obtenção do objeto, no caso pode haver um erro se for além dos limites do mundo.
 * @return Retorna o objeto.
 */
rf_ecosystem_object_t *rf_get_ecosystem_object(rf_ecosystem_t *es, int x, int y,
                                               error_t *status) {
  if (y < es->C && y >= 0 && x >= 0 && x < es->L) {
    *status = E_SUCCESS;
    return &(es->environment[x][y]);
  }
  *status = E_OUT_OF_BOUNDS;
  return NULL;
}

/**
 * @brief Enumera as células que não tem objetos de um certo tipo dado nas 4 direções.
 *
 * @param es Ambiente.
 * @param directions_available Direções disponíveis que serão preenchidas dentro da função.
 * @param directions_counter Enumeração das direções disponíveis que serão preenchidas dentro da função.
 * @param line Linha a ser avaliada as células adjacentes.
 * @param column Coluna a ser avaliada as células adjacentes.
 * @param obj_to_search_type Tipo de objeto a ser procurado.
 */
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

/**
 * @brief Atualiza os coelhos do ecossistema de acordo com as regras definidas na especificação.
 *
 * @param es Ecossistema base.
 * @param buffer_es Ecossistema que serve como um buffer para posterior atualização do ecossistema base.
 */
void rf_update_ecosystem_rabbits(rf_ecosystem_t *es,
                                 rf_ecosystem_t *buffer_es) {
  int line, column, z;
  int line1, column1;
  int directions_available[DIRECTIONS_NUMBER];
  int directions_counter;
  int next_cell_value;
  directions_t direction_tmp;
#ifdef PARALLEL
#pragma omp for
#endif
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
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[next_gen_rabbit.previous_line][next_gen_rabbit.previous_column]);
#endif
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
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[next_gen_rabbit.previous_line][next_gen_rabbit.previous_column]);
#endif
              }

#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line1][column1]);
#endif
              if (buffer_es->environment[line1][column1].type == RF_RABBIT) {

                if (buffer_es->environment[line1][column1].procreation_age <
                    next_gen_rabbit.procreation_age) {
                  buffer_es->environment[line1][column1] = next_gen_rabbit;
                }
              } else {
                buffer_es->environment[line1][column1] = next_gen_rabbit;
              }
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line1][column1]);
#endif
              break;
            }
          }
        } else {
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line][column]);
#endif
          buffer_es->environment[line][column] = next_gen_rabbit;
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line][column]);
#endif
        }
      }
    }
  }
}
/** 
 * @brief Atualiza as raposas do ecossistema de acordo com as regras definidas na especificação.
 *
 * @param es Ecossistema base.
 * @param buffer_es Ecossistema que serve como um buffer para posterior atualização do ecossistema base.
 */
void rf_update_ecosystem_foxes(rf_ecosystem_t *es, rf_ecosystem_t *buffer_es) {

  int line, column, z;
  int line1, column1;
  int directions_available[DIRECTIONS_NUMBER];
  int directions_counter;
  int next_cell_value;
  directions_t direction_tmp;

#ifdef PARALLEL
#pragma omp for
#endif
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
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line1][column1]);
#endif
              if (buffer_es->environment[line1][column1].type == RF_RABBIT) {
                next_gen_fox.food_generations = 0;
                insert_fox_in_environment = true;
              }
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line1][column1]);
#endif

              if (next_gen_fox.food_generations < es->GEN_COMIDA_RAPOSAS) {
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line1][column1]);
#endif
                if (buffer_es->environment[line1][column1].type == RF_EMPTY) {
                  insert_fox_in_environment = true;
                } else if (buffer_es->environment[line1][column1].type ==
                           RF_FOX) {
                  if (buffer_es->environment[line1][column1].procreation_age <
                      next_gen_fox.procreation_age) { // gets the fox with the
                    // oldest procreation_age
                    insert_fox_in_environment = true;
                  } else if (buffer_es->environment[line1][column1]
                                 .procreation_age ==
                             next_gen_fox
                                 .procreation_age) { // tie procreation_age
                    if (buffer_es->environment[line1][column1]
                            .food_generations >
                        next_gen_fox.food_generations) { // solves with
                                                         // food_generations
                      insert_fox_in_environment = true;
                    }
                  }
                }
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line1][column1]);
#endif
                if ((next_gen_fox.procreation_age - 1) ==
                    buffer_es->GEN_PROC_RAPOSAS) {

#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[next_gen_fox.previous_line][next_gen_fox.previous_column]);
#endif
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
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[next_gen_fox.previous_line][next_gen_fox.previous_column]);
#endif
                  next_gen_fox.procreation_age = 0;
                }
                if (insert_fox_in_environment) {
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line1][column1]);
#endif
                  buffer_es->environment[line1][column1] = next_gen_fox;
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line1][column1]);
#endif
                }
              } else {
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line1][column1]);
#endif
                buffer_es->environment[line1][column1].type = RF_EMPTY;
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line1][column1]);
#endif
              }
              break;
            }
          }
        } else {
#ifdef PARALLEL
       omp_set_lock(&LOCK_MATRIX[line][column]);
#endif
          buffer_es->environment[line][column] = next_gen_fox;
#ifdef PARALLEL
       omp_unset_lock(&LOCK_MATRIX[line][column]);
#endif
        }
      }
    }
  }
}
/** 
 * @brief Atualiza os coelhos do ecossistema para o ecossistema base.
 *
 * @param es Ecossistema base.
 * @param buffer_es Ecossistema que serve como um buffer que terá seus dados transferidos para o ecossistema base.
 */
void rf_update_ecosystem_rabbits_from_buffer(rf_ecosystem_t *es,
                                             rf_ecosystem_t *buffer_es) {
  int line, column;
  /*int N=0;*/

  COUNTER =0;

#ifdef PARALLEL
#pragma omp barrier
#endif

#ifdef PARALLEL
#pragma omp for reduction(+:COUNTER)
#endif
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_RABBIT) {
        es->environment[line][column].type = RF_EMPTY;
        COUNTER--;
      }
    }
  }
#ifdef PARALLEL
#pragma omp for reduction(+:COUNTER)
#endif
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (buffer_es->environment[line][column].type == RF_RABBIT) {
        es->environment[line][column] = buffer_es->environment[line][column];
        /*es->N++;*/
        COUNTER++;
      }
    }
  }

#ifdef PARALLEL
#pragma omp single
#endif
  es->N += COUNTER;
}

/** 
 * @brief Atualiza as raposas do ecossistema para o ecossistema base.
 *
 * @param es Ecossistema base.
 * @param buffer_es Ecossistema que serve como um buffer que terá seus dados transferidos para o ecossistema base.
 */
void rf_update_ecosystem_foxes_from_buffer(rf_ecosystem_t *es,
                                           rf_ecosystem_t *buffer_es) {
  int line, column;
  rf_ecosystem_object_t buffer_es_obj_tmp;
  COUNTER =0;

#ifdef PARALLEL
#pragma omp barrier
#endif
#ifdef PARALLEL
#pragma omp for reduction(+:COUNTER)
#endif
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      if (es->environment[line][column].type == RF_FOX) {
        es->environment[line][column].type = RF_EMPTY;
        COUNTER--;
      }
    }
  }
#ifdef PARALLEL
#pragma omp for reduction(+:COUNTER)
#endif
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      buffer_es_obj_tmp = buffer_es->environment[line][column];
      if (buffer_es_obj_tmp.type == RF_FOX) {
        if (es->environment[line][column].type != RF_EMPTY) {
          es->N--;
        }
        es->environment[line][column] = buffer_es->environment[line][column];
        COUNTER++;
      }
    }
  }
#ifdef PARALLEL
#pragma omp single
#endif
  es->N += COUNTER;
}

/** 
 * @brief Atualiza o ecossistema de maneira geral em uma geração.
 *
 * @param es Ecossistema base.
 * @param buffer_es Ecossistema que serve como um buffer.
 */
void rf_update_ecosystem_generation(rf_ecosystem_t *es,
                                    rf_ecosystem_t *buffer_es) {

  rf_update_ecosystem_rabbits(es, buffer_es);

#ifdef PARALLEL
#pragma omp barrier
#endif
  rf_update_ecosystem_rabbits_from_buffer(es, buffer_es);
#ifdef PARALLEL
#pragma omp barrier
#endif
  rf_update_ecosystem_foxes(es, buffer_es);
#ifdef PARALLEL
#pragma omp barrier
#endif
  rf_update_ecosystem_foxes_from_buffer(es, buffer_es);
}

/**
 * @brief Atualiza o ecossistema de maneira gereral com base nas gerações do ecossistema base de entrada.
 *
 * @param es Ecossistema base.
 * @param buffer_es Ecossistema que serve como um buffer.
 */
rf_ecosystem_t *rf_update_ecosystem_generations(rf_ecosystem_t *es) {
  rf_ecosystem_t *buffer_es = rf_clone_ecosystem(es);
  int i = 0;
  int generations_number = es->N_GEN;

/*omp_lock_t *lock = (omp_lock_t *)malloc((es->L) (es->C)*sizeof(omp_lock_t))*/

#ifdef PARALLEL
  LOCK_MATRIX = malloc(sizeof(omp_lock_t *) * es->L);
  for (int i = 0; i < es->L; i++) {
    LOCK_MATRIX[i] = malloc(sizeof(omp_lock_t) * es->C);
    for (int j = 0; j < es->C; j++) {
       omp_init_lock(&LOCK_MATRIX[i][j]);
       omp_unset_lock(&LOCK_MATRIX[i][j]);
    }
  }
#endif

#ifndef NDEBUG
  printf("============= [%d] Generation =============\n", i);
  rf_print_ecosystem_environment(es);
#endif

#ifdef PARALLEL
#pragma omp parallel private(i)
{
#endif
  for (i = 1; i <= generations_number; i++) {

#ifdef PARALLEL
    #pragma omp single
#endif
    es->current_generation = i - 1;

#ifdef PARALLEL
    #pragma omp single
#endif
    buffer_es->current_generation = i;


#ifdef PARALLEL
#pragma omp barrier
#endif

    rf_clear_environment(buffer_es);

#ifdef PARALLEL
#pragma omp barrier
#endif

    rf_update_ecosystem_generation(es, buffer_es);


#ifndef NDEBUG
    printf("============= [%d] Generation =============\n", i);
    rf_print_ecosystem_environment(es);
#endif

#ifdef PARALLEL
#pragma omp single
#endif
    es->N_GEN--;

  }
  
#ifdef PARALLEL
}
#endif

  rf_free_ecosystem(buffer_es);
#ifdef PARALLEL
  for (int i = 0; i < es->L; i++) {
    for (int j = 0; j < es->C; j++) {
       omp_destroy_lock(&LOCK_MATRIX[i][j]);
    }
    free(LOCK_MATRIX[i]);
  }
  free(LOCK_MATRIX);
  LOCK_MATRIX = NULL;
#endif
  return es;
}
