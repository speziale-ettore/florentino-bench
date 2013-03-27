
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "florentino/benchmark-runner.h"

// Port of John McCalpin's STREAM benchmark. The original benchmark employs
// statically sized arrays and target CPU. The benchmark has been extended such
// as the size of the array is configurable from command line. This can prevent
// aggressive compiler optimization, so I applied some optimization by hand.
namespace florentino {

class StreamBenchmarkRunner : public BenchmarkRunner {
public:
  static const size_t DEFAULT_ARRAY_LENGTH;
  static const size_t DEFAULT_DEVS_COUNT;
  static const std::string DEFAULT_DATA_DIR;

public:
  StreamBenchmarkRunner(int argc, char *argv[]);

public:
  // Size of the array used by STREAM to measure memory bandwidth. Please notice
  // that should be at least twice the size of the LLC in order to avoid caching
  // effects.
  size_t arrayLength() const { return _arrayLength; }

  // Some version of this benchmark -- e.g. OpenCL -- can exploit multiple
  // devices. This is a command line configurable parameter.
  size_t devsCount() const { return _devsCount; }

  // Some version of this benchmark -- e.g. OpenCL -- requires to load data from
  // files in order to run. This parameter identify the directory where look for
  // these files.
  const std::string &dataDir() const { return _dataDir; }

private:
  size_t _arrayLength;
  size_t _devsCount;
  std::string _dataDir;
};

// The structure of STREAM is very simple: the following member-wise operations
// should be performed:
//
// - copy
// - scale
// - add
// - triad
//
// Subclasses must implement them. This class just implement logging and it
// drives benchmark execution. Please notice you have to implement the init
// member function in order to fill arrays with initial values. That operation
// is not timed.
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
  // Utility method that perform benchmark validation on the host.
  void check(const double *a, const double *b, const double *c, double k);
};

inline std::ostream &hline(std::ostream &os) {
  for(unsigned i = 0, e = 62; i != e; ++i)
    os << "-";
  os << std::endl;

  return os;
}

} // End namespace florentino.

#endif // BENCHMARK_H
