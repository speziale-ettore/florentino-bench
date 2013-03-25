
#include "florentino/benchmark-runner.h"

#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>

using namespace florentino;

//
// Benchmark implementation.
//

void Benchmark::execute() {
  _clocks.record(ClkStart);
  run();
  _clocks.record(ClkEnd);
}

void Benchmark::setup() { }
void Benchmark::teardown() { }

void Benchmark::report() {
  typedef Benchmark::iterator clk_iterator;
  typedef TimeStat::iterator tks_iterator;

  // Print all clock values.
  for(clk_iterator i = _clocks.begin(), e = _clocks.end(); i != e; ++i)
    for(tks_iterator j = i->begin(), f = i->end(); j != f; ++j)
      log() << " " << std::scientific << *j;

  // Here we known the meaning of only the first two clocks, hence we can
  // perform only one subtraction. Subclasses can compute other stats.
  const TimeStat &stat = _clocks[ClkEnd] - _clocks[ClkStart];

  for(tks_iterator i = stat.begin(), e = stat.end(); i != e; ++i)
    log() << " " << std::scientific << *i;
  log() << " " << std::scientific << stat.avg();
}

std::ostream &Benchmark::log() const { return _runner->log(); }

#ifdef HAVE_OPENCL

//
// OpenCLAdapter implementation.
//

void OpenCLAdapter::allocDevices(cl_device_type devType, unsigned devsCount) {
  typedef std::vector<cl::Platform>::iterator iterator;

  std::vector<cl::Platform> plats;
  cl::Platform::get(&plats);

  iterator i, e;

  // Look for devices of the given type.
  for(i = plats.begin(), e = plats.end(); i != e && _devs.empty(); ++i) {
    // Try loading devices.
    try {
      i->getDevices(devType, &_devs);

      if(_devs.size() < devsCount)
        _devs.clear();
      else
        _devs.resize(devsCount);

    // Ignore the exception, just try with the next platform.
    } catch(...) {
      _devs.clear();
    }
  }

  // No devices available: error.
  if(_devs.empty()) {
    std::ostringstream os;
    os << "Cannot find " << devsCount << " "
                         << devType << (devsCount == 1 ? "" : "s");

    throw std::runtime_error(os.str());
  }

  // Save platform.
  _plat = *(i - 1);

  // Build a context for all these devices.
  cl_context_properties props[] =
    { CL_CONTEXT_PLATFORM,
      reinterpret_cast<cl_context_properties>(_plat()),
      0
    };
  _ctx = cl::Context(_devs, props);
}

void OpenCLAdapter::clearDevices() {
  // Try releasing alla resources. Ignore errors, we just want to reset the
  // state of this object.

  try {
    _ctx = cl::Context();
  } catch(...) { }

  try {
    _plat = cl::Platform();
  } catch(...) { }

  try {
    _devs.clear();
  } catch(...) { }

  try {
    _prog = cl::Program();
  } catch(...) { }
}

cl::Buffer OpenCLAdapter::allocBuffer(size_t size) {
  assert(_plat() && _ctx() && "unknown platform/context");

  return cl::Buffer::Buffer(_ctx, CL_MEM_READ_WRITE, size);
}

cl::CommandQueue OpenCLAdapter::allocQueue(unsigned dev) {
  assert(_plat() && _ctx() && "unknown platform/context");
  assert(dev < _devs.size() && "invalid device id");

  return cl::CommandQueue(_ctx, _devs[dev]);
}

void OpenCLAdapter::compile(const std::string &dataDir,
                            const std::string &file) {
  std::string path(dataDir + "/" + file);
  std::ifstream is(path.c_str());

  if(!is) {
    std::ostringstream os;
    os << "Error: cannot open '" << path << "'";

    throw std::runtime_error(os.str());
  }

  std::string src;

  try {
    // Note: the double '(' and ')' are really needed: do not remove!
    std::string src((std::istreambuf_iterator<char>(is)),
                    (std::istreambuf_iterator<char>()));
    cl::Program::Sources srcs(1, std::make_pair(src.c_str(), 0));

    _prog = cl::Program(_ctx, srcs);
    _prog.build(_devs);

  } catch(...) {
    std::ostringstream os;
    os << "Error: cannot compile '" << path << "'";

    throw std::runtime_error(os.str());
  }
}

cl::Kernel OpenCLAdapter::load(const std::string &name) {
  try {
    return cl::Kernel(_prog, name.c_str());

  } catch(...) {
    std::ostringstream os;
    os << "Error: cannot load kernel '" << name << "'";

    throw std::runtime_error(os.str());
  }
}

size_t OpenCLAdapter::preferredWGSizeMultiple(cl::Kernel &kernel,
                                              unsigned dev) {
  assert(_plat() && _ctx() && "unknown platform/context");
  assert(dev < _devs.size() && "invalid device id");

  return kernel.getWorkGroupInfo<CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE>(
           _devs[dev]);
}

#endif // HAVE_OPENCL
