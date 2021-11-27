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

/**
 * @brief get max value of a int vector
 *
 * @param vector
 * @param num_elements
 * @return int
 */
int get_max(int *vector, int num_elements) {
  int max = vector[0];
  for (int i = 1; i < num_elements; i++) {
    if (vector[i] > max) {
      max = vector[i];
    }
  }
  return max;
}

rf_ecosystem_object_type_t string_to_rf_ecosystem_object(char *string_buffer) {
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
 * @brief Initial setup of the programs, it manages I/O (parameters and input
 * files)
 *
 * @param argc
 * @param **argv
 * @param num_integers number of integers (to be updated) of the file
 * @return integers read from the input file
 */
rf_ecosystem_t* initial_setup(int argc, char **argv) {
  int num, i;
  if (argc != 1) {
    printf("Usage:\n");
    printf("%s <input file> <output file>\n", argv[0]);
    printf("Examples:\n");
    printf("%s entrada.txt saida.txt\n", argv[0]);
    printf("%s in.txt out1.txt\n", argv[0]);
    printf("%s in.txt out2.txt\n", argv[0]);
    exit(1);
  }
  /*char *fin_name = argv[1];*/
  /*FILE *fin = fopen(fin_name, "r");*/
  /*if (fin == NULL) {*/
    /*fprintf(stderr, "Unable to open file %s\n", fin_name);*/
    /*exit(1);*/
  /*}*/
  // create integers vector
  int num_lines = file_num_lines(stdin);

  int GEN_PROC_COELHOS, GEN_PROC_RAPOSAS, GEN_COMIDA_RAPOSAS, N_GEN, L, C, N;
  fscanf(stdin, "%d %d %d %d %d %d %d\n", &GEN_PROC_COELHOS, &GEN_PROC_RAPOSAS,
         &GEN_COMIDA_RAPOSAS, &N_GEN, &L, &C, &N);
  rf_ecosystem_t *es = rf_new_ecosystem(GEN_PROC_COELHOS, GEN_PROC_RAPOSAS,
                                         GEN_COMIDA_RAPOSAS, N_GEN, L, C, N);
  /*int *integers = malloc(sizeof(int) * num_lines);*/
  // read integers from file
  //
  char string_buffer[255];
  i = 0;
  int x, y;
  rf_ecosystem_object_type_t eco_type_obj;
  rf_ecosystem_object_t eco_obj;

  while (fscanf(stdin, "%s %d %d", string_buffer, &x, &y) > 0) {
    eco_type_obj = string_to_rf_ecosystem_object(string_buffer);
    /*obj.type = eco_obj;*/
    eco_obj.type=eco_type_obj;
    rf_insert_object_ecosystem(es, eco_obj, x, y);
    i++;
    if(i == N){
#ifndef NDEBUG
      printf("Reached max number of objects, stopping reading\n");
#endif
      break;
    }
  }

#ifndef NDEBUG
  printf("%d lines\n", num_lines);
#endif

  /**max_number = get_max(integers, *num_integers);*/

  /*#ifndef NDEBUG*/
  /*printf("maximum number: %d\n", *max_number);*/
  /*#endif*/
  return es;
}

/**
 * @brief Write the result in to the output file
 *
 * @param fout_name Name of the output file
 * @param integers_num_divisors List of the number of divisors of the integers
 * @param num_integers Number of integers
 */
void write_result(char *fout_name, int *integers_num_divisors,
                  int num_integers) {
  FILE *fout = fopen(fout_name, "w");
  if (fout == NULL) {
    fprintf(stderr, "Unable to open file %s\n", fout_name);
    exit(1);
  }
  for (int i = 0; i < num_integers; i++) {
    fprintf(fout, "%d\n", integers_num_divisors[i]);
  }
  fclose(fout);
}
