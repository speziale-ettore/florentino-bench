
#include "benchmarks.h"

using namespace florentino;

int main(int argc, char *argv[]) {
  StreamBenchmarkRunner runner(argc, argv);

  runner.add(new CPUStream(runner.arrayLength(), runner.log()));

  return runner.run();
}
