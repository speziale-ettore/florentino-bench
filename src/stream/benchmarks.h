
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "florentino/benchmark-runner.h"

namespace florentino {

class StreamBenchmarkRunner : public BenchmarkRunner {
public:
  static const size_t DEFAULT_ARRAY_LENGTH = 24 / sizeof(double) * size_t(1e6);

public:
  StreamBenchmarkRunner(int argc, char *argv[]);

public:
  size_t arrayLength() const { return _arrayLength; }

private:
  size_t _arrayLength;
};

class StreamBench : public Benchmark {
protected:
  StreamBench(size_t arrayLength, const std::string &nm, std::ostream &log)
    : Benchmark(nm, log),
    _arrayLength(arrayLength) { }

public:
  virtual void setup();
  virtual void run();
  virtual void teardown();

protected:
  virtual void init() = 0;
  virtual void copy() = 0;
  virtual void scale(double k) = 0;
  virtual void add() = 0;
  virtual void triad(double k) = 0;

  virtual void check(double k) const = 0;

protected:
  void check(const double *a, const double *b, const double *c, double k) const;

protected:
  const size_t _arrayLength;
};

} // End namespace florentino.

#endif // BENCHMARK_H
