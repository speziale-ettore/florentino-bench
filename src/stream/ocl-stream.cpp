
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
                            0,                   \
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

    cl::CommandQueue &queue = _envs[i].queue();
    cl::Kernel &init = _envs[i].init();

    init.setArg(0, _envs[i].a());
    init.setArg(1, _envs[i].b());
    init.setArg(2, _envs[i].c());
    init.setArg(3, cl_uint(myChunkLength));

    queue.enqueueNDRangeKernel(init,
                               cl::NullRange,
                               _envs[i].initGlobalWI(),
                               _envs[i].initLocalWI());
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

    cl::CommandQueue &queue = _envs[i].queue();
    cl::Kernel &copy = _envs[i].copy();

    copy.setArg(0, _envs[i].a());
    copy.setArg(1, _envs[i].c());
    copy.setArg(2, cl_uint(myChunkLength));

    queue.enqueueNDRangeKernel(copy,
                               cl::NullRange,
                               _envs[i].copyGlobalWI(),
                               _envs[i].copyLocalWI());
  }
}

void OpenCLGPUStream::scale(double k) {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::CommandQueue &queue = _envs[i].queue();
    cl::Kernel &scale = _envs[i].scale();

    scale.setArg(0, _envs[i].b());
    scale.setArg(1, _envs[i].c());
    scale.setArg(2, cl_double(k));
    scale.setArg(3, cl_uint(myChunkLength));

    queue.enqueueNDRangeKernel(scale,
                               cl::NullRange,
                               _envs[i].scaleGlobalWI(),
                               _envs[i].scaleLocalWI());
  }
}

void OpenCLGPUStream::add() {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::CommandQueue &queue = _envs[i].queue();
    cl::Kernel &add = _envs[i].add();

    add.setArg(0, _envs[i].a());
    add.setArg(1, _envs[i].b());
    add.setArg(2, _envs[i].c());
    add.setArg(3, cl_uint(myChunkLength));

    queue.enqueueNDRangeKernel(add,
                               cl::NullRange,
                               _envs[i].addGlobalWI(),
                               _envs[i].addLocalWI());
  }
}

void OpenCLGPUStream::triad(double k) {
  size_t chunkLength = arrayLength() / devsCount();

  for(unsigned i = 0, e = devsCount(); i != e; ++i) {
    size_t myChunkLength = chunkLength;

    if(i == e - 1)
      myChunkLength += arrayLength() % devsCount();

    cl::CommandQueue &queue = _envs[i].queue();
    cl::Kernel &triad = _envs[i].triad();

    triad.setArg(0, _envs[i].a());
    triad.setArg(1, _envs[i].b());
    triad.setArg(2, _envs[i].c());
    triad.setArg(3, cl_double(k));
    triad.setArg(4, cl_uint(myChunkLength));

    queue.enqueueNDRangeKernel(triad,
                               cl::NullRange,
                               _envs[i].triadGlobalWI(),
                               _envs[i].triadLocalWI());
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
