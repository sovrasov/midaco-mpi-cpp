#pragma once

#include <vector>
#include <string>
#include <cmdline.h>
#include <json.hpp>
#include <functional>
#include <cmath>
#include <chrono>

namespace
{

void saveStatistics(const std::vector<std::vector<int>>& stat,
                                  double avgTime, const cmdline::parser& parser,
                                  const std::string& capture = "")
{
  size_t numFuncs = stat.back().size() - 1;
  std::vector<double> avgCalcs(numFuncs, 0.);
  std::vector<double> stdCalcs(numFuncs, 0.);
  unsigned solvedCounter = 0;
  unsigned maxIters = 0;

  for(const auto& elem : stat)
  {
    maxIters = std::max(maxIters, (unsigned)elem[0]);
    for(size_t j = 0; j < numFuncs; j++)
      if(elem.back())
        avgCalcs[j] += elem[j];
    solvedCounter += elem.back();
  }
  for(size_t j = 0; j < numFuncs; j++)
    avgCalcs[j] /= solvedCounter;
  for(const auto& elem : stat)
  {
    for(size_t j = 0; j < numFuncs; j++)
      if(elem.back())
        stdCalcs[j] += pow(elem[j] - avgCalcs[j], 2);
  }
  for(size_t j = 0; j < numFuncs; j++)
  {
    stdCalcs[j] = sqrt(stdCalcs[j] / (solvedCounter + 1e-10));
    std::cout << "Average calculations number of function # " << j << " = "
              <<   avgCalcs[j] << " +- " << stdCalcs[j] << "\n";
  }
  std::cout << "Problems solved: " << solvedCounter << "\n";
  std::cout << "Maximum number of iterations: " << maxIters << "\n";

  if(parser.exist("saveStat"))
  {
    std::vector<std::pair<int, int>> operationCharacteristic;
    const unsigned opStep = maxIters / 150;
    for(unsigned i = 0; i < maxIters + opStep; i+= opStep)
    {
      int solvedProblemsCnt = 0;
      for(const auto& elem : stat)
        if(elem.back() && elem[numFuncs - 1] <= (int)i)
          solvedProblemsCnt++;
      operationCharacteristic.push_back(std::make_pair(i, solvedProblemsCnt));
    }

    auto fileName = parser.get<std::string>("outFile");
    const std::string sep = "_";
    const std::string stopType = parser.exist("maxEvalsStop") ? "maxEvals" : "optPoint";
    std::string generatedName = parser.get<std::string>("problemsClass") + sep +
      "n_" + std::to_string(parser.get<unsigned>("dim")) + sep +
      "eps_" + std::to_string(parser.get<double>("accuracy"));
    if(fileName.empty())
      fileName = generatedName + ".json";

    std::ofstream fout;
    fout.open(fileName, std::ios_base::out);
    if (!fout.is_open()) {
      std::cerr << "Failed to create file " << fileName << std::endl;
      return;
    }
    nlohmann::json j;

    j["num_solved"] = solvedCounter;
    j["num_solved"] = solvedCounter;
    j["calc_counters_std"] = stdCalcs;
    j["calc_counters"] = avgCalcs;
    j["capture"] = "MIDACO OpenMP";
    j["avg_time"] = avgTime;

    std::vector<int> cmc_iters;
    std::vector<double> cmc_values;
    for(const auto& point : operationCharacteristic)
    {
      cmc_iters.push_back(point.first);
      cmc_values.push_back(point.second / static_cast<double>(stat.size()));
    }
    j["cmc_iters"] = cmc_iters;
    j["cmc_vals"] = cmc_values;

    fout << j;
  }
}

std::function<double()> buildComputeLoad(double delay)
{
  if (delay == 0)
    return [] {return 1;};

  double estimatedTime = 0;
  unsigned complexity = 0;
  unsigned delta = 100;

  auto computeKernel = [](unsigned iters)
  {
    double value = 0;
    for (unsigned i = 0; i < iters; i++)
    {
      double a1 = sin(value + i);
      double a2 = cos(value + i);
      value = a2*a2 + a1*a1;
    }
    return value + 1.;
  };

  double val = 0;
  do
  {
    complexity += delta;
    auto start = std::chrono::system_clock::now();
    for(int i = 0; i < 100; i++)
    {
      val = computeKernel(complexity);
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    estimatedTime = elapsed_seconds.count() / 100;
  }
  while(estimatedTime * 1000. < delay);
  std::cout << "Estimated delay: " << estimatedTime*1000 << "\t complexity: " << complexity << "\t kernel value: " << val << '\n';

  return std::bind(computeKernel, complexity);
}

}
