
#ifndef CPU_STREAM_H
#define CPU_STREAM_H

#include "benchmarks.h"

#include <cstdlib>

namespace florentino {

class CPUStream : public StreamBench {
public:
  CPUStream(size_t arrayLength, std::ostream &log)
    : StreamBench(arrayLength, "CPU-STREAM", log),
      _a(_arrayLength),
      _b(_arrayLength),
      _c(_arrayLength) { }

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

  void check(double k) const;

private:
  std::vector<double> _a;
  std::vector<double> _b;
  std::vector<double> _c;
};

} // End namespace florentino.

#endif // CPU_STREAM_H
