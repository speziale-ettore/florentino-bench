
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "florentino/benchmark-runner.h"

#include "cpu-stream.h"

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

} // End namespace florentino.

#endif // BENCHMARK_H
