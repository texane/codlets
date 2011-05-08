#include <stdio.h>
#include "fixed.h"
#include <stdint.h>
#include <math.h>


int main(int ac, char** av)
{
#if 1
  {
    fixed_t fu = float_to_fixed(1.2);
    fixed_t bar = float_to_fixed(2.4);
    fixed_t baz = fixed_add(fu, bar);

    printf("%f + ", fixed_to_float(fu));
    printf("%f = ", fixed_to_float(bar));
    printf("%f\n", fixed_to_float(baz));
  }

  {
    fixed_t fu = float_to_fixed(1.5);
    fixed_t bar = int_to_fixed(2);
    fixed_t baz = fixed_mul(fu, bar);

    printf("%f * ", fixed_to_float(fu));
    printf("%f = ", fixed_to_float(bar));
    printf("%f\n", fixed_to_float(baz));
  }

  {
    fixed_t fu = int_to_fixed(1);
    fixed_t bar = int_to_fixed(2);
    fixed_t baz = fixed_div(fu, bar);

    printf("%f / ", fixed_to_float(fu));
    printf("%f = ", fixed_to_float(bar));
    printf("%f\n", fixed_to_float(baz));
  }

  {
    fixed_t fu = int_to_fixed(-1);
    fixed_t bar = int_to_fixed(2);
    fixed_t baz = fixed_div(fu, bar);

    printf("%f / ", fixed_to_float(fu));
    printf("%f = ", fixed_to_float(bar));
    printf("%f\n", fixed_to_float(baz));
  }
#endif

#if 0
  {
    fixed_t alpha = float_to_fixed(0.0);
    fixed_t step = float_to_fixed(0.01);
    for (; alpha < FIXED_TWO_PI; alpha = fixed_add(alpha, step))
    {
      printf("%f ", fixed_to_float(alpha));
      printf("%f\n", fabsf(sinf(fixed_to_float(alpha)) - fixed_to_float(fixed_sin(alpha))));
    }

  }
#endif

  return 0;
}
