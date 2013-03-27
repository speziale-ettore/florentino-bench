
#ifndef OCL_STREAM_H
#define OCL_STREAM_H

#ifdef HAVE_OPENCL

#include "benchmarks.h"

namespace florentino {

// Execute STREAM on an OpenCL device. Boilerplate code to setup the OpenCL
// context and doing data transfer is defined in this class. Subclasses must
// define and launch the actual STREAM kernels.
class OpenCLStream : public StreamBench,
                     public OpenCLAdapter {
public:
  // This class stores everything needed by each OpenCL device to execute the
  // kernels. I had to do some MACRO programming, but it looks this was the only
  // way to keep code clean.
  class Environment {
  public:
    #define BUFFER(N)                               \
    void N(const cl::Buffer &buf) { _ ## N = buf; }

    BUFFER(a)
    BUFFER(b)
    BUFFER(c)

    #undef BUFFER

    void queue(const cl::CommandQueue &qeu) { _queue = qeu; }

    #define KERNEL(N)                                           \
    void N(cl::Kernel &kern, size_t globalWI, size_t localWI) { \
      _ ## N = kern;                                            \
      _ ## N ## GlobalWI = cl::NDRange(globalWI);               \
      _ ## N ## LocalWI = cl::NDRange(localWI);                 \
    }

    KERNEL(init)
    KERNEL(copy)
    KERNEL(scale)
    KERNEL(add)
    KERNEL(triad)

    #undef KERNEL

    void clear() {
      try {
        *this = Environment();
      } catch(...) { }
    }

  public:
    #define BUFFER(N)                  \
    cl::Buffer &N() { return _ ## N; }

    BUFFER(a)
    BUFFER(b)
    BUFFER(c)

    #undef BUFFER

    cl::CommandQueue &queue() { return _queue; }

    #define KERNEL(N)                                           \
    cl::Kernel &N() { return _ ## N; }                          \
    cl::NDRange &N ## GlobalWI() { return _ ## N ## GlobalWI; } \
    cl::NDRange &N ## LocalWI() { return _ ## N ## LocalWI; }

    KERNEL(init)
    KERNEL(copy)
    KERNEL(scale)
    KERNEL(add)
    KERNEL(triad)

    #undef KERNEL

  private:
    #define BUFFER(N)  \
    cl::Buffer _ ## N;

    BUFFER(a)
    BUFFER(b)
    BUFFER(c)

    #undef BUFFER

    cl::CommandQueue _queue;

    #define KERNEL(N)               \
    cl::Kernel _ ## N;              \
    cl::NDRange _ ## N ## GlobalWI; \
    cl::NDRange _ ## N ## LocalWI;

    KERNEL(init)
    KERNEL(copy)
    KERNEL(scale)
    KERNEL(add)
    KERNEL(triad)

    #undef KERNEL
  };

protected:
  OpenCLStream(const std::string &nm,
               cl_device_type devType,
               StreamBenchmarkRunner &runner)
    : StreamBench(nm, runner),
      _devType(devType) { }

public:
  virtual void setup();
  virtual void teardown();

protected:
  virtual cl::Kernel loadInit() = 0;
  virtual cl::Kernel loadCopy() = 0;
  virtual cl::Kernel loadScale() = 0;
  virtual cl::Kernel loadAdd() = 0;
  virtual cl::Kernel loadTriad() = 0;

  virtual void check(double k);

protected:
  cl_device_type _devType;
  std::vector<Environment> _envs;
};

// STREAM benchmark for OpenCL-enabled GPUs.
class OpenCLGPUStream : public OpenCLStream {
public:
  OpenCLGPUStream(StreamBenchmarkRunner &runner)
    : OpenCLStream("OCL-GPU", CL_DEVICE_TYPE_GPU, runner) { }

protected:
  virtual cl::Kernel loadInit() { return load("gpu_init"); }
  virtual cl::Kernel loadCopy() { return load("gpu_copy"); }
  virtual cl::Kernel loadScale() { return load("gpu_scale"); }
  virtual cl::Kernel loadAdd() { return load("gpu_add"); }
  virtual cl::Kernel loadTriad() { return load("gpu_triad"); }

protected:
  virtual void init();
  virtual void copy();
  virtual void scale(double k);
  virtual void add();
  virtual void triad(double k);
};

} // End namespace florentino.

#endif // HAVE_OPENCL

#endif // OCL_STREAM_H
