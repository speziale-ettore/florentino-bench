
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "florentino/benchmark-runner.h"

namespace florentino {

class StreamBenchmarkRunner : public BenchmarkRunner {
public:
  static const size_t DEFAULT_ARRAY_LENGTH;
  static const size_t DEFAULT_DEVS_COUNT;
  static const std::string DEFAULT_DATA_DIR;

public:
  StreamBenchmarkRunner(int argc, char *argv[]);

public:
  size_t arrayLength() const { return _arrayLength; }
  size_t devsCount() const { return _devsCount; }
  const std::string &dataDir() const { return _dataDir; }

private:
  size_t _arrayLength;
  size_t _devsCount;
  std::string _dataDir;
};

class StreamBench : public Benchmark {
protected:
  StreamBench(const std::string &nm, StreamBenchmarkRunner &runner)
    : Benchmark(nm, runner) { }

public:
  virtual void setup();
  virtual void run();
  virtual void teardown();

public:
  size_t arrayLength() const {
    StreamBenchmarkRunner &runner = Benchmark::runner<StreamBenchmarkRunner>();
    return runner.arrayLength();
  }

  size_t devsCount() const {
    StreamBenchmarkRunner &runner = Benchmark::runner<StreamBenchmarkRunner>();
    return runner.devsCount();
  }

  const std::string &dataDir() const {
    StreamBenchmarkRunner &runner = Benchmark::runner<StreamBenchmarkRunner>();
    return runner.dataDir();
  }

protected:
  virtual void init() = 0;
  virtual void copy() = 0;
  virtual void scale(double k) = 0;
  virtual void add() = 0;
  virtual void triad(double k) = 0;

  virtual void check(double k) = 0;

protected:
  void check(const double *a, const double *b, const double *c, double k);
};

} // End namespace florentino.

#endif // BENCHMARK_H
