
#ifndef CPU_STREAM_H
#define CPU_STREAM_H

#include "florentino/benchmark.h"

#include <cstdlib>

namespace florentino {

class CPUStream : public Benchmark {
public:
  CPUStream(size_t arrayLength, std::ostream &log)
    : Benchmark("CPU-STREAM", log),
      _arrayLength(arrayLength),
      _a(_arrayLength),
      _b(_arrayLength),
      _c(_arrayLength) { }

private:
  CPUStream(const CPUStream &that); // Do not implement.
  const CPUStream &operator=(const CPUStream &that); // Do not implement.

public:
  virtual void setup();
  virtual void run();
  virtual void teardown();

private:
  void init();
  void copy();
  void scale(double k);
  void add();
  void triad(double k);

  void check(double k);

private:
  size_t _arrayLength;

  std::vector<double> _a;
  std::vector<double> _b;
  std::vector<double> _c;
};

} // End namespace florentino.

#endif // CPU_STREAM_H
