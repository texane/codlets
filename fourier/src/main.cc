#include <stdio.h>
#include <math.h>
#include <vector>

typedef double real_type;

static void gen_func
(std::vector<real_type>& f, real_type msec, real_type fsamp, real_type fosc)
{
  const unsigned int nsamp = ::ceil(fsamp * msec / 1000);
  f.resize(nsamp);

  // total radians during the sampling period: 2 * PI * fosc * msecs / 1000
  // radians per sampling ticks: above / nsamp
  const real_type da = (2 * M_PI * fosc * msec / 1000) / (real_type)nsamp;
  for (unsigned int i = 0; i < nsamp; ++i) f[i] = ::cos(da * (real_type)i);
}

static void add_func
(std::vector<real_type>& f, real_type msec, real_type fsamp, real_type fosc)
{
  const unsigned int nsamp = ::ceil(fsamp * msec / 1000);
  const real_type da = (2 * M_PI * fosc * msec / 1000) / (real_type)nsamp;
  for (unsigned int i = 0; i < nsamp; ++i) f[i] += ::cos(da * (real_type)i);
}

__attribute__((unused))
static void plot_func
(const std::vector<real_type>& f)
{
  printf("x = [");
  for (unsigned int i = 0; i < f.size(); ++i) printf(" %lf", f[i]);
  printf(" ]\n");
}

static void dft
(const std::vector<real_type>& f, std::vector<real_type>& c, real_type fsamp)
{
  // compute the discrete fourier coefficients
  // f the function
  // c the fourier coefficients
  // fsamp the sampling frequency

  c.resize(f.size());

  // the coefficient index
  for (unsigned int n = 0; n < f.size(); ++n)
  {
    // integrate
    c[n] = 0;
    for (unsigned int i = 0; i < f.size() - 1; ++i)
    {
      const real_type a = -2 * M_PI * (real_type)i * (real_type)n / (real_type)f.size();
      c[n] += f[i] * ::cos(a);
      // c[n] -= f[i] * ::sin(a);
    }
  }
}

static void compute_power_spectrum
(std::vector<real_type>& k)
{
  // k the fourier coefficients

  const unsigned int n = k.size();
  for (unsigned int i = 1; i < (n + 1) / 2; ++i)
    k[i] = k[i] * k[i] + k[n - i] * k[n - i];
}

#ifndef CONFIG_USE_IDFT
# define CONFIG_USE_IDFT 1
#endif

#if CONFIG_USE_IDFT
static void idft
(std::vector<real_type>& f, std::vector<real_type>& c, real_type freq)
{
}
#endif // CONFIG_USE_IDFT

int main(int ac, char** av)
{
  static const real_type fosc = 10; // oscillating freq
  static const real_type fsamp = fosc * 4 + 1; // sampling freq
  static const real_type msec = 1000; // sampling time

  std::vector<real_type> f;
  std::vector<real_type> c;

  gen_func(f, msec, fsamp, fosc);
  add_func(f, msec, fsamp, 3);
//   add_func(f, msec, fsamp, fsamp / 2 - 1);
//   plot_func(f);

  dft(f, c, fsamp);
  compute_power_spectrum(c);

  for (unsigned int i = 0; i < c.size(); ++i)
    printf("%lf %lf\n", fsamp / c.size() * i, c[i]);

#if CONFIG_USE_IDFT
  std::vector<real_type> i;
  i.resize(f.size());
  idft(f, c, fsamp);
#endif // CONFIG_USE_IDFT

//   for (unsigned int i = 0; i < c.size(); ++i) printf("%u %lf\n", i, c[i]);

  return 0;
}
