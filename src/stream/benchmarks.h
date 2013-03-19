
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "florentino/benchmark-runner.h"

#include "cpu-stream.h"

namespace florentino {

class StreamBenchmarkRunner : public BenchmarkRunner {
public:
  static const size_t DEFAULT_ARRAY_LENGTH = 8 * 1024 * 1024;

public:
  StreamBenchmarkRunner(int argc, char *argv[])
    : BenchmarkRunner(argc, argv),
      _arrayLength(DEFAULT_ARRAY_LENGTH) { }

public:
  size_t getArrayLength() const { return _arrayLength; }

private:
  size_t _arrayLength;
};

} // End namespace florentino.

#endif // BENCHMARK_H
