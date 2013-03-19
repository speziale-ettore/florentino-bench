
#include "florentino/benchmark-runner.h"

#include <cstdlib>

using namespace florentino;

BenchmarkRunner::BenchmarkRunner(int argc, char **argv) : _argc(argc),
                                                          _argv(argv) { }

BenchmarkRunner::~BenchmarkRunner() {
  typedef std::vector<Benchmark *>::iterator iterator;

  for(iterator i = _benchmarks.begin(), e = _benchmarks.end(); i != e; ++i)
    delete *i;
}

int BenchmarkRunner::run() {
  return EXIT_FAILURE;
}
