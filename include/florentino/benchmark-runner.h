
#ifndef FLORENTINO_BENCHMARK_RUNNER_H
#define FLORENTINO_BENCHMARK_RUNNER_H

#include <florentino/benchmark.h>

#include <vector>

namespace florentino {

class BenchmarkRunner {
public:
  BenchmarkRunner(int argc, char **argv);
  ~BenchmarkRunner();

private:
  // Do not implement.
  BenchmarkRunner(const BenchmarkRunner &that);

  // Do not implement.
  const BenchmarkRunner &operator=(const BenchmarkRunner &that);

public:
  void add(Benchmark *bench) { _benchmarks.push_back(bench); }

  int run();

private:
  int _argc;
  char **_argv;

  std::vector<Benchmark *> _benchmarks;
};

} // End namespace florentino.

#endif // FLORENTINO_BENCHMARK_RUNNER_H
