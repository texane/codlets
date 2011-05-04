/* compile with:
   gcc -Wall -O3 -march=native main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

static inline void sort2i(const int* v, unsigned int* k)
{
  /* sort 2 values such that v[k[0]] > v[k[1]] */
  k[0] = 0;
  k[1] = 0;
  k[((v[1] - v[0]) & (1U << 31)) >> 31] = 1;
}

static inline void sort2i_(const int* v, unsigned int* k, unsigned int i)
{
  /* sort 2 values such that v[i + k[i + 0]] > v[i + k[i + 1]] */
  const unsigned int ying = k[i + 0];
  const unsigned int yang = k[i + 1];
  k[i + 1] = ying;
  k[i + (((v[yang] - v[ying]) & (1U << 31)) >> 31)] = yang;
}

static inline void sort3i(const int* v, unsigned int* k)
{
  /* sort 3 values such that v[k[0]] > v[k[1]] > v[k[2]] */

  k[0] = 0;
  k[1] = 1;
  k[2] = 2;

  sort2i_(v, k, 0);
  sort2i_(v, k, 1);
  sort2i_(v, k, 0);
}

static inline void sort3i_orig(const int* v, unsigned int* k)
{
  k[0] = 0;
  k[1] = 1;
  k[2] = 2;

#define swap(__a, __b)				\
  do {						\
    const unsigned int __tmp = __a;		\
    __a = __b;					\
    __b = __tmp;				\
  } while (0)

  if (v[k[1]] > v[k[0]]) swap(k[0], k[1]);
  if (v[k[2]] > v[k[0]]) swap(k[0], k[2]);
  if (v[k[2]] > v[k[1]]) swap(k[1], k[2]);
}

static inline void force_use(unsigned int* k)
{
  __asm__ __volatile__ ("nop"::"m"(&k[0]));
}

int main(int ac, char** av)
{
  unsigned int keys[3];
  int vals[3];

  unsigned int iter;
  struct timeval sta, sto, dif;

  vals[0] = 200 + rand() % 100;
  vals[1] = 100 + rand() % 100;
  vals[2] = 300 + rand() % 100;

  gettimeofday(&sta, NULL);

  for (iter = 0; iter < 200000; ++iter)
  {
    /* sort3i(vals, keys); */
    sort3i_orig(vals, keys);
    force_use(keys);
    /* printf("%d %d %d -> ", vals[0], vals[1], vals[2]); */
    /* printf("%d %d %d\n", vals[keys[0]], vals[keys[1]], vals[keys[2]]); */
  }

  gettimeofday(&sto, NULL);

  timersub(&sto, &sta, &dif);
  printf("%lf\n", dif.tv_sec * 1E6 + dif.tv_usec);

  return 0;
}
