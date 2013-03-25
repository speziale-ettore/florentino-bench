
#ifndef CPU_STREAM_H
#define CPU_STREAM_H

#include "benchmarks.h"

#include <cstdlib>

namespace florentino {

class CPUStream : public StreamBench {
public:
  CPUStream(StreamBenchmarkRunner &runner)
    : StreamBench("CPU-STREAM", runner) { }

protected:
  virtual void init();
  virtual void copy();
  virtual void scale(double k);
  virtual void add();
  virtual void triad(double k);

  virtual void check(double k);

private:
  std::vector<double> _a;
  std::vector<double> _b;
  std::vector<double> _c;
};

} // End namespace florentino.

#endif // CPU_STREAM_H
