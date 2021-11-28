#include "rabbits_fox.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
int main(void) {
  rf_ecosystem_t *es = initial_setup();

  clock_t begin = clock();
  rf_update_ecosystem_generations(es);
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

  print_ecosystem_full_report(es);

#ifdef TIME_DEBUG
  printf("Time spend with computation: %f\n", time_spent);
#endif
  rf_free_ecosystem(es);
  return 0;
}
