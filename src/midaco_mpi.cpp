#include "midaco_mpi.hpp"

#include <midaco_core.h>

MidacoSolution solve_midaco_mpi(const IGOProblem<double>* problem, const MidacoMPIParameters& params,
    std::function<bool(const double*)> external_stop)
{
  MidacoSolution solution;

  return solution;
}
