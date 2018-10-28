#pragma once

#include <vector>
#include <functional>

#include "problem_interface.hpp"

struct MidacoOMPParameters
{
  unsigned numThreads = 1;
  unsigned maxEvals = 1000;
  double focus = 0.;
  double seed = 0.;
};

struct MidacoSolution
{
  std::vector<double> optPoint;
  std::vector<double> optValues;
  std::vector<int> calcCounters;
};

MidacoSolution solve_midaco_omp(const IGOProblem<double>*, const MidacoOMPParameters& params,
      std::function<bool(const double*)> external_stop = [](const double*){ return false; });
