#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <unistd.h>

#define STEP 25

struct do_calc_args
{
  double *times;
  int i;
  int max;
  int nproc;
};

void *do_calc(void *vargp);
long long unsigned run_calc(int);

int main(int argc, char const *argv[])
{
  long nproc = sysconf(_SC_NPROCESSORS_ONLN);
  pthread_t *threads;
  double *times;

  if (argc != 2 || nproc <= 0)
    exit(1);
  int max = strtol(argv[1], NULL, 10);

  printf("Using %ld threads\n", nproc);
  FILE *file = fopen("number.csv", "w");
  fprintf(file, "");
  fclose(file);

  times = calloc(max, sizeof(double));
  if (times == NULL)
    exit(1);

  threads = calloc(nproc, sizeof(pthread_t));
  if (threads == NULL)
    exit(1);

  for (int i = 0; i < nproc; i++)
  {
    struct do_calc_args *args = malloc(sizeof(struct do_calc_args));
    assert(args != NULL);
    args->i = i;
    args->max = max;
    args->times = times;
    args->nproc = nproc;
    pthread_create(&threads[i], NULL, &do_calc, args);
  }

  for (int i = 0; i < nproc; i++)
    pthread_join(threads[i], NULL);

  free(times);
  free(threads);

  return 0;
}

void *do_calc(void *vargp)
{
  struct do_calc_args *args = (struct do_calc_args *)vargp;
  double *times = args->times;
  int j = args->i;
  int max = args->max;
  int nproc = args->nproc;
  for (int i = j; i < max; i += nproc)
  {
    times[i] = (double)run_calc(STEP * (i + 1)) / CLOCKS_PER_SEC;
    printf("Time for %ddp: %F\n", STEP * (i + 1), times[i]);
  }
  mpfr_free_cache();
  free(vargp);
  return NULL;
}

long long unsigned
run_calc(int dp)
{
  mpfr_t x, xold, tmp1, tmp2, precision_bits, e;
  long long unsigned wallclock;
  long long unsigned i = 0;
  long unsigned precision;
  FILE *file = fopen("number.csv", "a");
  fprintf(file, "%d, ", dp);

  /* Calculate precision */
  mpfr_init(precision_bits);
  mpfr_set_ui(precision_bits, 10, MPFR_RNDU);
  mpfr_pow_ui(precision_bits, precision_bits, dp, MPFR_RNDU);
  mpfr_log2(precision_bits, precision_bits, MPFR_RNDU);
  precision = mpfr_get_ui(precision_bits, MPFR_RNDU) + 1;
  printf("Using %lu precision bits (%.4FKB)\n", precision, (double)precision / (8 * 1024));

  /* Init X, tmp1, tmp2 */
  mpfr_inits2(precision, x, xold, tmp1, tmp2, e, NULL);
  mpfr_set_ui(x, 0, 0);
  mpfr_set_ui(xold, 1, 0);
  mpfr_set_ui(e, 10, 0);
  mpfr_pow_si(e, e, -(dp), 0);

  /* Perform the calculation */
  clock_t start = clock();
  while (1)
  {
    mpfr_sub(tmp1, x, xold, 0);
    if (mpfr_cmpabs(e, tmp1) >= 0)
      break;

    i++;
    mpfr_set(xold, x, 0);
    mpfr_add_ui(tmp1, x, 1, 0);
    mpfr_div(tmp2, x, tmp1, 0);
    mpfr_pow(x, tmp2, x, 0);
  }
  clock_t end = clock();
  wallclock = ((end - start));
  fprintf(file, "%llu, ", i);
  fprintf(file, "%.4F\n", (double)wallclock / CLOCKS_PER_SEC);
  fclose(file);

  mpfr_clears(x, xold, tmp1, tmp2, precision_bits, e, NULL);

  return wallclock;
}
