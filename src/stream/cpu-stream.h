
#ifndef CPU_STREAM_H
#define CPU_STREAM_H

#include "florentino/benchmark.h"

#include <cstdlib>

namespace florentino {

class CPUStream : public Benchmark {
public:
  CPUStream(size_t arrayLength) : _arrayLength(arrayLength) { }

private:
  CPUStream(const CPUStream &that); // Do not implement.
  const CPUStream &operator=(const CPUStream &that); // Do not implement.

public:
  virtual bool run();

private:
  size_t _arrayLength;
};

} // End namespace florentino.

#endif // CPU_STREAM_H
