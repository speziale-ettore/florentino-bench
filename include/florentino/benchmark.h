
#ifndef FLORENTINO_BENCHMARK_H
#define FLORENTINO_BENCHMARK_H

namespace florentino {

class Benchmark {
protected:
  Benchmark() { }

private:
  Benchmark(const Benchmark &that); // Do not implement.
  const Benchmark &operator=(const Benchmark &that); // Do not implement.

public:
  virtual void setup() { }
  virtual void teardown() { }

  virtual bool run() = 0;
};

} // End namespace florentino.

#endif // FLORENTINO_BENCHMARK_H
