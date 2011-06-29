#include <math.h>
#include <stdlib.h>
#include "SimulationViewer.hh"


// static configuration
#define CONFIG_CUBE_DIM 10
#define CONFIG_HEAT_K 0.1
#define CONFIG_SIMU_DT 100


// globals

static double* cubes[2];

static inline void swap_cubes(void)
{
  double* const tmp = cubes[0];
  cubes[0] = cubes[1];
  cubes[1] = tmp;
}


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

static void diffuse_heat
(
 const double* scube, double* dcube, unsigned int dim,
 unsigned int k, unsigned int dt
)
{
  // scube the source cube
  // dcube the dest cube
  // k the heat propagation factor
  // dt the time step
}

static void step(SimulationViewer* viewer)
{
  diffuse_heat();
  swap_cubes();
}

int main(int ac, char** av)
{
  // initialize 3d cube
  cubes[0] = alloc_cube(CONFIG_CUBE_DIM);
  zero_cube(cubes[0]);
  cubes[1] = alloc_cube(CONFIG_CUBE_DIM);

  // init and run viewer
  SimulationViewer* const viewer =
    SimulationViewer::makeSimulationViewer(ac, av, step);
  viewer->show();
  return viewer->execute();
}
