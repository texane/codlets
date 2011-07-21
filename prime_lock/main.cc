#include <stdio.h>
#include <stdlib.h>


// note: there is no need to actually allocate the prime array.
// we could only maintain the sieve array as a bitmap, requiring
// nk / 8 bytes of storage.

__attribute__((aligned(64)))
static volatile unsigned long value;
static unsigned long* primes;

static inline unsigned long key_to_prime(unsigned long k)
{
  return primes[k];
}

static void lock_init(unsigned long nk)
{
  const unsigned long size = nk * sizeof(unsigned long);
  unsigned long* const sieve = (unsigned long*)malloc(size);

  // nk the maximum key number
  primes = (unsigned long*)malloc(size);

  for (unsigned long k = 0; k < nk; ++k) sieve[k] = 1;

  sieve[0] = 0;
  sieve[1] = 0;

  unsigned long i = 0;
  for (unsigned long k = 2; k < nk; ++k)
  {
    for (; sieve[k] == 0 && k < nk; ++k) ;
    primes[i++] = k;
    for (unsigned long j = k * k; j < nk; j += k) sieve[j] = 0;
  }

  value = 1;

  free(sieve);
}

static void lock_fini(void)
{
  free(primes);
}

static void lock_acquire(unsigned long k)
{
  const unsigned long p = key_to_prime(k);

  unsigned long v;

 redo_mul:
  // wait for value not to contain p
  v = value;
  if ((v % p) == 0) goto redo_mul;
  // affect and retry on race
  if (__sync_bool_compare_and_swap(&value, v, p * v)) return ;
  goto redo_mul;
}

static void lock_release(unsigned long k)
{
  const unsigned long p = key_to_prime(k);

  unsigned long v = value;
 redo_div:
  const unsigned long prev =
    __sync_val_compare_and_swap(&value, v, v / p);
  if (prev == v) return ;
  v = prev;
  goto redo_div;
}

// unit

int main(int ac, char** av)
{
  lock_init(500000);

  lock_acquire(4);
  lock_acquire(2);

  lock_release(2);
  lock_release(4);

  lock_fini();

  return 0;
}
