
#include "ocl-stream.h"

#include <iomanip>

#ifdef HAVE_OPENCL

using namespace florentino;

//
// OCLStream implementation.
//

void OpenCLStream::setup() {
  _envs.resize(devsCount());

  size_t chunkLength = arrayLength() / devsCount();

  // Request superclass to find all devices we need.
  allocDevices(_devType, devsCount());

  // Stores information about WIs to be printed before execution.
  #define KERNEL(N)                                                 \
  std::vector<std::pair<size_t, size_t> > N ## Spaces(devsCount());

  KERNEL(init)
  KERNEL(copy)
  KERNEL(scale)
  KERNEL(add)
  KERNEL(triad)

  #undef KERNEL

  // Allocate needed resources.
  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    Environment &env = _envs[i];
    size_t myChunkLength = chunkLength;

    // Extra elements processed by the last device.
    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    #define BUFFER(N)                                   \
    env.N(allocBuffer(myChunkLength * sizeof(double)));

    BUFFER(a)
    BUFFER(b)
    BUFFER(c)

    #undef BUFFER

    env.queue(allocQueue(i));
  }

  // Compile the program.
  compile(dataDir(), "florentino-stream-kernels.cl");

  // Load kernels.
  cl::Kernel init = loadInit(),
             copy = loadCopy(),
             scale = loadScale(),
             add = loadAdd(),
             triad = loadTriad();

  // Setup iteration spaces.
  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    Environment &env = _envs[i];
    size_t myChunkLength = chunkLength;

    size_t globalWI, localWI;

    // Last device should handles extra elements.
    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    #define KERNEL(N)                                                         \
    /* Initial attempt to map the iteration space on the device. */           \
    globalWI = myChunkLength;                                                 \
    localWI = 4 * preferredWGSizeMultiple(N, i);                              \
                                                                              \
    /* Number of local work items must alwasy be lesser or equal to number */ \
    /* of global work items. */                                               \
    if(globalWI < localWI)                                                    \
      globalWI = localWI;                                                     \
                                                                              \
    /* Make sure number of global work items is a multiple of number of */    \
    /* local work items. */                                                   \
    if(size_t rem = globalWI % localWI)                                       \
      globalWI += localWI - rem;                                              \
                                                                              \
    /* Iter space should be printer later */                                  \
    N ## Spaces[i] = std::make_pair(globalWI, localWI);                       \
                                                                              \
    /* Bind kernel to iteration space. */                                     \
    env.N(N, globalWI, localWI);

    KERNEL(init)
    KERNEL(copy)
    KERNEL(scale)
    KERNEL(add)
    KERNEL(triad)

    #undef KERNEL
  }

  // Now, there is a working OpenCL environment.
  StreamBench::setup();

  log() << "Iteration spaces:"
        << std::endl;

  #define KERNEL(N)                                 \
  log() << std::setw(18)                            \
        << # N ": ";                                \
                                                    \
  for(unsigned i = 0, e = devsCount(); i != e; ++i) \
    log() << "(" << N ## Spaces[i].first << ","     \
                 << N ## Spaces[i].second           \
          << ")";                                   \
                                                    \
  log() << std::endl;

  KERNEL(init)
  KERNEL(copy)
  KERNEL(scale)
  KERNEL(add)
  KERNEL(triad)

  #undef KERNEL

  log() << hline;
}

void OpenCLStream::teardown() {
  // Do superclass work.
  StreamBench::teardown();

  // Detach all resources.
  for(unsigned i = 0, e = devsCount(); i != e; ++i)
    _envs[i].clear();

  // Destroy implict OpenCL resources.
  clearDevices();
}

void OpenCLStream::check(double k) {
  std::vector<double> a(arrayLength()),
                      b(arrayLength()),
                      c(arrayLength());

  size_t chunkLength = arrayLength() / devsCount(),
         chunkSize = chunkLength * sizeof(double);

  // Read buffers into temp arrays.
  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength,
           myChunkSize;

    if(i == e - 1)
      myChunkLength += chunkLength % devsCount();

    myChunkSize = myChunkLength * sizeof(double);

    cl::CommandQueue &queue = _envs[i].queue();

    #define BUFFER(N)                            \
    queue.enqueueReadBuffer(_envs[i].N(),        \
                            false,               \
                            i * chunkSize,       \
                            myChunkSize,         \
                            &N[i * chunkLength]);

    BUFFER(a)
    BUFFER(b)
    BUFFER(c)

    #undef BUFFER
  }

  // Wait for commands to finish.
  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    cl::CommandQueue &queue = _envs[i].queue();
    queue.finish();
  }

  // Do the check in the host.
  StreamBench::check(&a[0], &b[0], &c[0], k);
}

//
// OpenCLGPUStream implementation.
//

void OpenCLGPUStream::init() {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::KernelFunctor &init = _envs[i].init();
    init(_envs[i].a(), _envs[i].b(), _envs[i].c(), cl_uint(myChunkLength));
  }

  // Before proceeding into the times section, make sure intialization is done!
  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    cl::CommandQueue &queue = _envs[i].queue();
    queue.finish();
  }
}

void OpenCLGPUStream::copy() {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::KernelFunctor &copy = _envs[i].copy();
    copy(_envs[i].a(), _envs[i].c(), cl_uint(myChunkLength));
  }
}

void OpenCLGPUStream::scale(double k) {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::KernelFunctor &scale = _envs[i].scale();
    scale(_envs[i].b(), _envs[i].c(), cl_double(k), cl_uint(myChunkLength));
  }
}

void OpenCLGPUStream::add() {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::KernelFunctor &add = _envs[i].add();
    add(_envs[i].a(), _envs[i].b(), _envs[i].c(), cl_uint(myChunkLength));
  }
}

void OpenCLGPUStream::triad(double k) {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::KernelFunctor &triad = _envs[i].triad();
    triad(_envs[i].a(),
          _envs[i].b(),
          _envs[i].c(),
          cl_double(k),
          cl_uint(myChunkLength));
  }

  // Triad is the last kernel: flush all queues just to be sure commands are
  // moved to devices and wait for termination.

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    cl::CommandQueue &queue = _envs[i].queue();
    queue.flush();
  }

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    cl::CommandQueue &queue = _envs[i].queue();
    queue.finish();
  }
}

#endif // HAVE_OPENCL
