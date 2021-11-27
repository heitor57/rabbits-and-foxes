#include "rabbits_fox.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(int argc, char **argv) {
  int max_number, num_integers;
  int *integers;

  rf_ecosystem_t* es = initial_setup(argc, argv);

#ifndef NDEBUG
  rf_print_ecosystem_environment(es);
#endif
  /*max_number = get_max(integers, num_integers);*/

  clock_t begin = clock();
  rf_update_ecosystem_generations(es);
  /*int *integers_num_divisors = NULL;*/
  /*integers_num_divisors = dfpack_serial_df(integers, max_number, num_integers);*/
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Time spend with computation: %f\n", time_spent);
  /*write_result(argv[2], integers_num_divisors, num_integers);*/
  rf_free_ecosystem(es);
  return 0;
}
