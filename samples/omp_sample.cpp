#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <memory>
#include <cmath>

#include <gkls_function.hpp>
#include <grishagin_function.hpp>
#include <midaco_omp.hpp>
#include <cmdline.h>

void saveStatistics(const std::vector<std::vector<int>>& stat, const cmdline::parser& parser);
void initParser(cmdline::parser& parser);

int main(int argc, char** argv)
{
  cmdline::parser parser;
  initParser(parser);
  parser.parse_check(argc, argv);

  MidacoOMPParameters parameters;
  parameters.numThreads = parser.get<unsigned>("numThreads");
  double eps = parser.get<double>("accuracy");
  parameters.maxEvals = parser.get<unsigned>("maxEvals");
  bool maxEvalsStop = parser.exist("maxEvalsStop");

  std::string problemClass = parser.get<std::string>("problemsClass");
  auto start = std::chrono::system_clock::now();
  std::vector<std::vector<int>> allStatistics;

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

    MidacoSolution solution;
    if (maxEvalsStop)
      solution = solve_midaco_omp(problem.get(), parameters);
    else
    {
      auto stop_criterion = [&opt, &lb, &ub, eps](const double* y)
      {
        for(size_t i = 0; i < opt.size(); i++)
          if (fabs(opt[i] - y[i]) > (ub[i] - lb[i])*eps)
            return false;
        return true;
      };
      solution = solve_midaco_omp(problem.get(), parameters, stop_criterion);
    }
    allStatistics.push_back(solution.calcCounters);

    bool isSolved = true;
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
  auto end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end - start;
  std::cout << "Time elapsed: " << elapsed_seconds.count() << "s\n";

  saveStatistics(allStatistics, parser);

  return 0;
}

void saveStatistics(const std::vector<std::vector<int>>& stat, const cmdline::parser& parser)
{
  size_t numFuncs = stat.back().size() - 1;
  std::vector<double> avgCalcs(numFuncs, 0.);
  unsigned solvedCounter = 0;
  unsigned maxIters = 0;

  for(const auto& elem : stat)
  {
    maxIters = std::max(maxIters, (unsigned)elem[0]);
    for(size_t j = 0; j < numFuncs; j++)
      avgCalcs[j] += elem[j];
    solvedCounter += elem.back();
  }
  for(size_t j = 0; j < numFuncs; j++)
  {
    avgCalcs[j] /= stat.size();
    std::cout << "Average calculations number of function # " << j << " = " << avgCalcs[j] << "\n";
  }
  std::cout << "Problems solved: " << solvedCounter << "\n";
  std::cout << "Maximum number of iterations: " << maxIters << "\n";

  /*
  if(parser.exist("saveStat"))
  {
    std::vector<std::pair<int, int>> operationCharacteristic;
    const unsigned opStep = maxIters / 150;
    for(unsigned i = 0; i < maxIters + opStep; i+= opStep)
    {
      int solvedProblemsCnt = 0;
      for(const auto& elem : stat)
        if(elem.back() && elem[numFuncs - 1] <= i)
          solvedProblemsCnt++;
      operationCharacteristic.push_back(std::make_pair(i, solvedProblemsCnt));
    }

    auto fileName = parser.get<std::string>("outFile");
    const std::string sep = "_";
    const std::string stopType = parser.exist("accuracyStop") ? "accuracy" : "optPoint";
    std::string generatedName = parser.get<std::string>("problemsClass") + sep +
      "n_" + std::to_string(parser.get<int>("dim")) + sep +
      "r_" + std::to_string(parser.get<double>("reliability")) + sep +
      "eps_" + std::to_string(parser.get<double>("accuracy")) + sep +
      "lm_" + std::to_string(parser.get<int>("localMix"));
    if(fileName.empty())
      fileName = generatedName + ".csv";

    std::ofstream fout;
    fout.open(fileName, std::ios_base::out);
    fout << generatedName << std::endl;
    for(const auto& point : operationCharacteristic)
      fout << point.first << ", " << point.second << std::endl;
  }
  */
}
void initParser(cmdline::parser& parser)
{
  parser.add<double>("accuracy", 'a', "accuracy of the method", false, 0.01);
  parser.add<unsigned>("maxEvals", 'e', "limit of evaluations for the method", false, 5000);
  parser.add<unsigned>("numThreads", 'p', "number of OMP threads", false, 1);
  parser.add<unsigned>("dim", 'd', "test problem dimension (will be set if supported)", false, 2);
  parser.add<std::string>("problemsClass", 'c', "Name of the used problems class", false,
    "gklsS", cmdline::oneof<std::string>("gklsS", "gklsH", "grish"));
  parser.add<std::string>("outFile", 'f', "Name of the output .csv file with statistics", false, "");
  parser.add("maxEvalsStop", 'm', "Stop by max evals");
  parser.add("saveStat", 's', "Save statistics in a .json file");
}
