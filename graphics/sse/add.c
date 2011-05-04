/*
  compile with:
  gcc -march=native -O3 add.c
  refer to:
  http://gcc.gnu.org/onlinedocs/gcc-4.0.1/gcc/Vector-Extensions.html
  /usr/lib/gcc/i486-linux-gnu/4.4/include/xmmintrin.h
 */

#include <stdio.h>
#include <xmmintrin.h>
#include <sys/time.h>

typedef float real_type;

int main(int ac, char** av)
{
  volatile __attribute__((aligned(16))) real_type fu[4];
  volatile __attribute__((aligned(16))) real_type ba[4];
  volatile __attribute__((aligned(16))) real_type de[4];

  unsigned int iter;
  struct timeval sta, sto, dif;

  for (iter = 0; iter < 4; ++iter)
  {
    fu[iter] = (real_type)(rand() % 1) + 1;
    ba[iter] = (real_type)(rand() % 1) + 1;
  }

  gettimeofday(&sta, NULL);

  for (iter = 0; iter < 2000000; ++iter)
  {
#if 1 /* CONFIG_USE_SSE */
    __m128 d;
    __m128 s;

    d = _mm_load_ps((real_type*)fu);
    s = _mm_load_ps((real_type*)ba);
    d = _mm_add_ps(d, s);
    _mm_store_ps((real_type*)de, d);
#else
    de[0] = fu[0] + ba[0];
    de[1] = fu[1] + ba[1];
    de[2] = fu[2] + ba[2];
    de[3] = fu[3] + ba[3];
#endif
  }

  gettimeofday(&sto, NULL);

  timersub(&sto, &sta, &dif);
  printf("%lf\n", dif.tv_sec * 1E6 + dif.tv_usec);

  return 0;
}
