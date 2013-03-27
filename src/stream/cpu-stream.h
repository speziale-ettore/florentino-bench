
#ifndef CPU_STREAM_H
#define CPU_STREAM_H

#include "benchmarks.h"

#include <cstdlib>

namespace florentino {

// Execute STREAM on the CPU, employing just 1 thread.
class CPUStream : public StreamBench {
public:
  CPUStream(StreamBenchmarkRunner &runner)
    : StreamBench("CPU-STREAM", runner),
      _a(0),
      _b(0),
      _c(0) { }

public:
  virtual void setup();
  virtual void teardown();

protected:
  virtual void init();
  virtual void copy();
  virtual void scale(double k);
  virtual void add();
  virtual void triad(double k);

  virtual void check(double k);

private:
  double *_a;
  double *_b;
  double *_c;
};

} // End namespace florentino.

#endif // CPU_STREAM_H
