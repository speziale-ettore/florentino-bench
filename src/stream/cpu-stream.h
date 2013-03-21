
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

protected:
  virtual void init();
  virtual void copy();
  virtual void scale(double k);
  virtual void add();
  virtual void triad(double k);

  virtual void check(double k) const;

private:
  std::vector<double> _a;
  std::vector<double> _b;
  std::vector<double> _c;
};

} // End namespace florentino.

#endif // CPU_STREAM_H
