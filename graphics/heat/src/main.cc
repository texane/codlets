#include <math.h>
#include <stdlib.h>
#include "SimulationViewer.hh"


// static configuration
#define CONFIG_CUBE_DIM 10
#define CONFIG_SIM_DT 0.1 // seconds
#define CONFIG_SIM_K 0.1


// cubes
static double* cubes[2];

// generate a cube where point are equally spaced

static inline double* alloc_cube(unsigned int dim)
{
  return (double*)malloc(dim * dim * dim * sizeof(double));
}

static void zero_cube(double* cube, unsigned int dim)
{
  for (unsigned int i = 0; i < dim; ++i, ++cube)
    *cube = 0;
}

static inline unsigned int to_index
(unsigned int i, unsigned int j, unsigned int k, unsigned int dim)
{
  return
}

static void diffuse_heat
(
 const double* u, double* uu, unsigned int dim,
 double k, double dt, double dx
)
{
  // scube the source cube
  // dcube the dest cube
  // k the heat propagation factor
  // dt the time step

  const unsigned int ddim = dim * dim;
  const double xx = x * x;
  const double k_dt_xx = k / dt * xx;
  const double c0 = 1 - 6 * k_dt_xx;
  const double c1 = k_dt_xx;

  unsigned int pos = 0;
  for (unsigned int i = 0; i < dim; ++i)
    for (unsigned int j = 0; j < dim; ++j)
      for (unsigned int k = 0; k < dim; ++k, ++pos)
      {
	u[pos] =
	  c0 * u[pos] + c1 *
	  (u[pos - dim] + u[pos + 1] + u[pos + dim] + u[pos - 1] + u[pos - ddim] + u[pos + ddim]);
      }
}

static void step(SimulationViewer* viewer)
{
  // diffuse heat
  diffuse_heat
    (cubes[0], cubes[1], CONFIG_CUBE_DIM, CONFIG_SIM_K, CONFIG_SIM_DT);

  // swap cubes
  double* const tmp = cubes[0];
  cubes[0] = cubes[1];
  cubes[1] = tmp;
}

int main(int ac, char** av)
{
  // initialize 3d cube
  cubes[0] = alloc_cube(CONFIG_CUBE_DIM);
  zero_cube(cubes[0]);
  cubes[1] = alloc_cube(CONFIG_CUBE_DIM);

  // init and run viewer
  static const unsigned int dt = (unsigned int)(CONFIG_SIM_DT * 1000);
  SimulationViewer* const viewer =
    SimulationViewer::makeSimulationViewer(ac, av, step, dt);
  viewer->show();
  return viewer->execute();
}
