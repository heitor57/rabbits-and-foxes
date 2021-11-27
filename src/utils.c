#include "errors.h"
#include "rabbits_fox.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief number of lines of a file
 *
 * @param f
 * @return int
 */
int file_num_lines(FILE *f) {
  char c;
  int lines = 0;
  while (!feof(f)) {
    c = fgetc(f);
    if (c == '\n') {
      lines++;
    }
  }
  fseek(f, 0, SEEK_SET);
  return lines;
}

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
 * @brief Initial setup of the programs, it manages I/O (parameters and input
 * files)
 *
 * @param argc
 * @param **argv
 * @param num_integers number of integers (to be updated) of the file
 * @return integers read from the input file
 */
rf_ecosystem_t *initial_setup() {
  int num, i;
  int num_lines = file_num_lines(stdin);

  int GEN_PROC_COELHOS, GEN_PROC_RAPOSAS, GEN_COMIDA_RAPOSAS, N_GEN, L, C, N;
  fscanf(stdin, "%d %d %d %d %d %d %d\n", &GEN_PROC_COELHOS, &GEN_PROC_RAPOSAS,
         &GEN_COMIDA_RAPOSAS, &N_GEN, &L, &C, &N);
  rf_ecosystem_t *es = rf_new_ecosystem(GEN_PROC_COELHOS, GEN_PROC_RAPOSAS,
                                        GEN_COMIDA_RAPOSAS, N_GEN, L, C, N);
  char string_buffer[255];
  i = 0;
  int x, y;
  rf_ecosystem_object_type_t eco_type_obj;
  rf_ecosystem_object_t eco_obj;

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
