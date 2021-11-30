#include "errors.h"
#include "rabbits_fox.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Transforma um tipo de objeto do ecosistema em uma string.
 *
 * @param object_type Tipo de objeto a ser "transformado" em uma string.
 * @return string que representa um tipo de objeto.
 */
char *
rf_ecosystem_object_type_to_string(rf_ecosystem_object_type_t object_type) {
  if (object_type == RF_ROCK) {
    return "ROCHA";
  } else if (object_type == RF_FOX) {
    return "RAPOSA";
  } else if (object_type == RF_RABBIT) {
    return "COELHO";
  } else {
    return "UNKNOWN";
  }
}

/**
 * @brief Transforma um tipo de objeto do ecosistema em uma string.
 *
 * @param string_buffer string que representa um tipo de objeto.
 * @return um tipo de objeto rf_ecosystem_object_type_t correspondente a string_buffer.
 */
rf_ecosystem_object_type_t
string_to_rf_ecosystem_object_type(char *string_buffer) {
  if (!strcmp(string_buffer, "ROCHA")) {
    return RF_ROCK;
  } else if (!strcmp(string_buffer, "RAPOSA")) {
    return RF_FOX;
  } else if (!strcmp(string_buffer, "COELHO")) {
    return RF_RABBIT;
  } else {
    return RF_EMPTY;
  }
}

/**
 * @brief Imprime o ecosistema na saída padrão no formato esperado definido na especificação.
 *
 * @param es Ecosistema para ser imprimido.
 */
void print_ecosystem_full_report(rf_ecosystem_t *es) {

  fprintf(stdout, "%d %d %d %d %d %d %d\n", es->GEN_PROC_COELHOS,
          es->GEN_PROC_RAPOSAS, es->GEN_COMIDA_RAPOSAS, es->N_GEN, es->L, es->C,
          es->N);

  int line = 0;
  int column = 0;
  rf_ecosystem_object_t const *obj;
  for (line = 0; line < es->L; line++) {
    for (column = 0; column < es->C; column++) {
      obj = &es->environment[line][column];
      if (obj->type != RF_EMPTY) {
        printf("%s %d %d\n", rf_ecosystem_object_type_to_string(obj->type),
               line, column);
      }
    }
  }
}

/**
 * @brief Configuração inicial dos programas, no qual é feito uma leitura da entrada padrão e criado o ecosistema inicial.
 * @return Ecossistema carregado da entrada padrão.
 */
rf_ecosystem_t *initial_setup() {
  int i;

  int GEN_PROC_COELHOS, GEN_PROC_RAPOSAS, GEN_COMIDA_RAPOSAS, N_GEN, L, C, N;
  fscanf(stdin, "%d %d %d %d %d %d %d\n", &GEN_PROC_COELHOS, &GEN_PROC_RAPOSAS,
         &GEN_COMIDA_RAPOSAS, &N_GEN, &L, &C, &N);
  rf_ecosystem_t *es = rf_new_ecosystem(GEN_PROC_COELHOS, GEN_PROC_RAPOSAS,
                                        GEN_COMIDA_RAPOSAS, N_GEN, L, C, N);

  char string_buffer[255];
  int x, y;
  rf_ecosystem_object_type_t eco_type_obj;
  rf_ecosystem_object_t eco_obj;
  i = 0;

  while (fscanf(stdin, "%s %d %d", string_buffer, &x, &y) > 0) {
    eco_type_obj = string_to_rf_ecosystem_object_type(string_buffer);
    eco_obj.type = eco_type_obj;
    rf_insert_object_ecosystem(es, eco_obj, x, y);
    i++;
    if (i == N) {
      break;
    }
  }
  return es;
}
