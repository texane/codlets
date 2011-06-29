#include <math.h>
#include <stdlib.h>
#include "SimulationViewer.hh"


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

static void step(void)
{
  std::cout << "fu" << std::endl;
}

int main(int ac, char** av)
{
  SimulationViewer* const viewer =
    SimulationViewer::makeSimulationViewer(ac, av, step);
  viewer->show();
  return viewer->execute();
}
