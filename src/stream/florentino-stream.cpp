
#include "cpu-stream.h"
#include "ocl-stream.h"

using namespace florentino;

int main(int argc, char *argv[]) {
  StreamBenchmarkRunner runner(argc, argv);

  runner.add(new CPUStream(runner));

#ifdef HAVE_OPENCL
  runner.add(new OpenCLGPUStream(runner));
#endif

  return runner.run();
}
