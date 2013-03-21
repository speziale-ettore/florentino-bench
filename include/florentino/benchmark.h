
#ifndef FLORENTINO_BENCHMARK_H
#define FLORENTINO_BENCHMARK_H

#include <florentino/clock.h>

#include <iostream>

namespace florentino {

class BenchmarkRunner;

class Benchmark {
public:
  enum {
    ClkStart,
    ClkEnd
  };

  typedef Clocks::iterator iterator;

public:
  iterator begin() const { return _clocks.begin(); }
  iterator end() const { return _clocks.end(); }

protected:
  Benchmark(const std::string &nm = "UNKNOWN", std::ostream &log = std::clog)
    : _name(nm),
      _log(&log) {
    _clocks.reserve(ClkStart, "start");
    _clocks.reserve(ClkEnd, "end");
  }

private:
  Benchmark(const Benchmark &that); // Do not implement.
  const Benchmark &operator=(const Benchmark &that); // Do not implement.

public:
  virtual ~Benchmark() { }

public:
  void execute();

  virtual void setup();
  virtual void teardown();
  virtual void report();

public:
  const std::string &name() const { return _name; }

protected:
  virtual void run() = 0;

  std::ostream &log() const {
    return *_log;
  }

  size_t runs() const {
    return std::min(_clocks[ClkStart].size(), _clocks[ClkEnd].size());
  }

protected:
  Clocks _clocks;

private:
  std::string _name;
  std::ostream *_log;
};

} // End namespace florentino.

#endif // FLORENTINO_BENCHMARK_H
