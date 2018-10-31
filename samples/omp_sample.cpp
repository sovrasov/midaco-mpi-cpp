#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <memory>
#include <cmath>

#include <gkls_function.hpp>
#include <grishagin_function.hpp>
#include <midaco_omp.hpp>

#include "common.hpp"

void initParser(cmdline::parser& parser);

int main(int argc, char** argv)
{
  cmdline::parser parser;
  initParser(parser);
  parser.parse_check(argc, argv);

  MidacoOMPParameters parameters;
  parameters.focus = parser.get<int>("focus");
  parameters.numThreads = parser.get<unsigned>("numThreads");
  double eps = parser.get<double>("accuracy");
  parameters.maxEvals = parser.get<unsigned>("maxEvals");
  bool maxEvalsStop = parser.exist("maxEvalsStop");

  std::string problemClass = parser.get<std::string>("problemsClass");
  std::vector<std::vector<int>> allStatistics;
  double totalTime = 0.0;

  for (int i = 0; i < 100; i++)
  {
    std::shared_ptr<IGOProblem<double>> problem;
    if (problemClass == "gklsS" || problemClass == "gklsH")
    {
      auto *func = new gkls::GKLSFunction();
      if (problemClass == "gklsS")
        func->SetFunctionClass(gkls::Simple, parser.get<unsigned>("dim"));
      else
        func->SetFunctionClass(gkls::Hard, parser.get<unsigned>("dim"));

      func->SetType(gkls::TD);
      func->SetFunctionNumber(i + 1);
      problem = std::shared_ptr<IGOProblem<double>>(func);
    }
    else if (problemClass == "grish")
    {
      auto *func = new vagrish::GrishaginFunction();
      func->SetFunctionNumber(i + 1);
      problem = std::shared_ptr<IGOProblem<double>>(func);
    }
    std::vector<double> opt(problem->GetDimension());
    problem->GetOptimumPoint(opt.data());
    std::vector<double> lb(problem->GetDimension()), ub(problem->GetDimension());
    problem->GetBounds(lb.data(), ub.data());

    auto start = std::chrono::system_clock::now();
    MidacoSolution solution;
    bool isSolved = false;
    if (maxEvalsStop)
      solution = solve_midaco_omp(problem.get(), parameters);
    else
    {
      auto stop_criterion = [&isSolved, &opt, &lb, &ub, eps](const double* y)
      {
        for(size_t i = 0; i < opt.size(); i++)
          if (fabs(opt[i] - y[i]) > (ub[i] - lb[i])*eps)
            return false;
        isSolved = true;
        return true;
      };
      solution = solve_midaco_omp(problem.get(), parameters, stop_criterion);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    totalTime += elapsed_seconds.count();
    allStatistics.push_back(solution.calcCounters);

    if (maxEvalsStop)
      for(size_t i = 0; i < opt.size(); i++)
        if (fabs(opt[i] - solution.optPoint[i]) > (ub[i] - lb[i])*eps)
          isSolved = false;
    std::cout << "Problem # " << i + 1;
    if (isSolved)
    {
      std::cout << " solved.";
      allStatistics.back().push_back(1);
    }
    else
    {
      std::cout << " not solved.";
      allStatistics.back().push_back(0);
    }
    std::cout << " Iterations performed: " << allStatistics.back()[0] << "\n";
  }

  std::cout << "Time elapsed: " << totalTime << "s\n";
  std::cout << "Avg time per problem: " << totalTime / 100. << "s\n";

  saveStatistics(allStatistics, totalTime / 100., parser, "MIDACO OMP");

  return 0;
}

void initParser(cmdline::parser& parser)
{
  parser.add<double>("accuracy", 'a', "accuracy of the method", false, 0.01);
  parser.add<int>("focus", 0, "", false, -1);
  parser.add<unsigned>("maxEvals", 'e', "limit of evaluations for the method", false, 5000);
  parser.add<unsigned>("numThreads", 'p', "number of OMP threads", false, 1);
  parser.add<unsigned>("dim", 'd', "test problem dimension (will be set if supported)", false, 2);
  parser.add<std::string>("problemsClass", 'c', "Name of the used problems class", false,
    "gklsS", cmdline::oneof<std::string>("gklsS", "gklsH", "grish"));
  parser.add<std::string>("outFile", 'f', "Name of the output .csv file with statistics", false, "");
  parser.add("maxEvalsStop", 'm', "Stop by max evals");
  parser.add("saveStat", 's', "Save statistics in a .json file");
}
