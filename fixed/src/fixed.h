#ifndef FIXED_H_INCLUDED
# define FIXED_H_INCLUDED


typedef int fixed_t;


#define FIXED_N_SIZE (20)
#define FIXED_N_MASK ((1 << FIXED_N_SIZE) - 1)
#define FIXED_N_MAX (1 << FIXED_N_SIZE)

#define FIXED_F_SIZE (32 - FIXED_N_SIZE)
#define FIXED_F_MASK ((1 << FIXED_F_SIZE) - 1)
#define FIXED_F_MAX (1 << FIXED_F_SIZE)

#define FIXED_ONE int_to_fixed(1)
#define FIXED_PI float_to_fixed(3.14159265)
#define FIXED_TWO_PI float_to_fixed(2 * 3.14159265)
#define FIXED_HALF_PI float_to_fixed(3.14159265 / 2)


static inline fixed_t float_to_fixed(float f)
{
  return ((int)(f * (float)(1 << FIXED_F_SIZE)));
}

static inline fixed_t int_to_fixed(int i)
{
  return (fixed_t)(i << FIXED_F_SIZE);
}

static inline int fixed_to_int(fixed_t f)
{
  return f >> FIXED_F_SIZE;
}

static inline float fixed_to_float(fixed_t f)
{
  return (((float)f) / (float)(1 << FIXED_F_SIZE));
}

static inline fixed_t fixed_add(fixed_t a, fixed_t b)
{
  return a + b;
}

static inline fixed_t fixed_sub(fixed_t a, fixed_t b)
{
  return a - b;
}

static inline fixed_t fixed_mul(fixed_t a, fixed_t b)
{
  /* assume no overflow */
  return (fixed_t)((a * b) >> FIXED_F_SIZE);
}

static inline fixed_t fixed_div(fixed_t a, fixed_t b)
{
  /* assume no overflow */
  return (fixed_t)((a << FIXED_F_SIZE) / b);
}

static inline fixed_t fixed_sin(fixed_t fp)
{
  int sign = 1;
  fixed_t sqr;
  fixed_t result;
  int SK[2];

  SK[0] = 16342350 >> (31 - FIXED_F_SIZE);
  SK[1] = 356589659 >> (31 - FIXED_F_SIZE);

  fp %= 2 * FIXED_PI;

  if (fp < 0)
    fp = FIXED_PI * 2 + fp;

  if ((fp > FIXED_HALF_PI) && (fp <= FIXED_PI))
  {
    fp = FIXED_PI - fp;
  }
  else if ((fp > FIXED_PI) && (fp <= (FIXED_PI + FIXED_HALF_PI)))
  {
    fp = fp - FIXED_PI;
    sign = -1;
  }
  else if (fp > (FIXED_PI + FIXED_HALF_PI))
  {
    fp = (FIXED_PI << 1) - fp;
    sign = -1;
  }

  sqr = fixed_mul(fp, fp);

  result = SK[0];
  result = fixed_mul(result, sqr);
  result -= SK[1];
  result = fixed_mul(result, sqr);
  result += FIXED_ONE;
  result = fixed_mul(result, fp);

  return sign * result;
}


#endif /* ! FIXED_H_INCLUDED */
