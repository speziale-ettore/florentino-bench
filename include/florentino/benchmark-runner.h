
#ifndef FLORENTINO_BENCHMARK_RUNNER_H
#define FLORENTINO_BENCHMARK_RUNNER_H

#include <florentino/benchmark.h>
#include <florentino/logstream.h>
#include <florentino/option-parser.h>

#include <vector>

namespace florentino {

// Manages everything about the run of a set of benchmark. If you want to add
// new options, just subclass it and use:
//
// void BenchmarkRunner::add(const Option &).
//
// To add new benchmarks to this runner, call:
//
// void BenchmarkRunner::add(Benchmark *)
//
// At last, to run benchmarks call:
// new options, just subclass it.
//
// void BenchmarkRunner::run()
//
// All benchmarks gets run and stats are gathered.
class BenchmarkRunner {
public:
  static const unsigned DEFAULT_TIMES = 1;
  static const bool DEFAULT_VERBOSE = false;

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

public:
  std::ostream &log() const { return const_cast<logstream &>(_log); }

protected:
  void add(const Option &opt) { _options.add(opt); }

private:
  OptionParser _options;
  logstream _log;

  unsigned _times;
  bool _verbose;

  std::vector<Benchmark *> _benchmarks;
};

} // End namespace florentino.

#endif // FLORENTINO_BENCHMARK_RUNNER_H
