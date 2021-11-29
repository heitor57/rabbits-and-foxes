#include "rabbits_fox.h"
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#ifdef LPARALLEL
#include <omp.h>
#endif
int main(void) {
  rf_ecosystem_t *es = initial_setup();

  struct timeval start, end;
  gettimeofday(&start, NULL);
  /*clock_t begin = clock();*/
  rf_update_ecosystem_generations(es);
  /*clock_t end = clock();*/
  /*double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;*/
  /*#ifdef LPARALLEL*/
  /*[>time_spent =/ <]*/
  /*#endif*/
  gettimeofday(&end, NULL);
  double time_taken = end.tv_sec + end.tv_usec / 1e6 -
    start.tv_sec - start.tv_usec / 1e6; // in seconds

  print_ecosystem_full_report(es);

#ifdef TIME_DEBUG
  printf("Time spend with computation: %f\n", time_taken);
#endif
  rf_free_ecosystem(es);
  return 0;
}
