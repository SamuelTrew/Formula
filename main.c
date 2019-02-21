#include <stdio.h>
#include <mpfr.h>


int main(int argc, char const *argv[])
{
		mpfr_t x, a, b, c, d;
		unsigned i = 0;
		mpfr_set_default_prec(16384);
		mpfr_inits(x, a, b, c, d, NULL);
		mpfr_set_ui(x, 0, 0);
		printf("Precision: %li\n", mpfr_get_default_prec());
		while (i < 400000)
		{
				i++;
				mpfr_add_ui(a, x, 1, 0);
				mpfr_div(b, x, a, 0);
				mpfr_pow(c, b, x, 0);
				mpfr_set(x, c, 0);
		}
		FILE *file = fopen("Formula.txt", "w+");
		mpfr_fprintf(file, "%1.10000Rf", x);
		mpfr_printf("x = %1.10000Rf\n", x);
		return 0;
}