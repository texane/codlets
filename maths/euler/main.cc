// euler method to solve a first order differential equation of the form:
// f'(x) = g(x, f(x))
// where f(x) is the unknown function the method approximates.
// an initial condition is given:
// x0, f(x0)
// the solver produces an approximation for f(x) on the range [x0, xn[.

#include <stdio.h>
#include <stdlib.h>

typedef double real_type;

static inline unsigned int get_buffer_size
(real_type x0, real_type xn, real_type dh)
{
  return (unsigned int)((xn - x0) / dh) + 1;
}

static void euler_solve
(
 real_type (*g)(real_type, real_type),
 real_type x0, real_type y0,
 real_type dh, real_type xn,
 real_type* y
)
{
  // g the g function as mentionned above
  // x0, y0 the initial condition
  // dh the iteration step
  // xn the range value
  // y the approximated values. it must be large enough to hold
  // all the computed values, ie. (xn - x0) / dh

  y[0] = y0;

  if (x0 >= (xn - dh)) return ;

  for (; x0 < (xn - dh); x0 += dh, ++y)
    y[1] = g(x0, y[0]) * dh + y[0];
}


// unit

static real_type g(real_type x, real_type y)
{
  return x / 2;
}

static inline real_type F(real_type x)
{
  // the primitive
  return x * x / 4;
}

int main(int ac, char** av)
{
#define CONFIG_X0 1
#define CONFIG_Y0 0.25
#define CONFIG_DH 0.1
#define CONFIG_XN 3

  const unsigned int size = get_buffer_size(CONFIG_X0, CONFIG_XN, CONFIG_DH);
  real_type* const y = (real_type*)malloc(size * sizeof(real_type));

  euler_solve(g, CONFIG_X0, CONFIG_Y0, CONFIG_DH, CONFIG_XN, y);

  for (unsigned int i = 0; i < size; ++i)
  {
    const real_type xi = CONFIG_X0 + (real_type)i * CONFIG_DH;
    printf("%lf %lf %lf\n", xi, y[i], F(xi));
  }

  free(y);

  return 0;
}
