
#ifndef FLORENTINO_BENCHMARK_H
#define FLORENTINO_BENCHMARK_H

#include <florentino/clock.h>

#include <iostream>

#ifdef HAVE_OPENCL

#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#endif //HAVE_OPENCL

namespace florentino {

class BenchmarkRunner;

class Benchmark {
public:
  enum {
    ClkStart,
    ClkEnd
  };

  typedef Clocks::iterator iterator;

public:
  iterator begin() const { return _clocks.begin(); }
  iterator end() const { return _clocks.end(); }

protected:
  Benchmark() : _name("UNKNOWN"),
                _runner(0) { }

  Benchmark(const std::string &nm, BenchmarkRunner &runner)
    : _name(nm),
      _runner(&runner) {
    _clocks.reserve(ClkStart, "start");
    _clocks.reserve(ClkEnd, "end");
  }

private:
  Benchmark(const Benchmark &that); // Do not implement.
  const Benchmark &operator=(const Benchmark &that); // Do not implement.

public:
  virtual ~Benchmark() { }

public:
  void execute();

  virtual void setup();
  virtual void teardown();
  virtual void report();

public:
  const std::string &name() const {
    return _name;
  }

  size_t runs() const {
    return std::min(_clocks[ClkStart].size(), _clocks[ClkEnd].size());
  }

protected:
  virtual void run() = 0;

  std::ostream &log() const;

  BenchmarkRunner &runner() const {
    return *_runner;
  }

  template <typename Ty>
  Ty &runner() const {
    return *reinterpret_cast<Ty *>(_runner);
  }

protected:
  Clocks _clocks;

private:
  std::string _name;
  BenchmarkRunner *_runner;
};

#ifdef HAVE_OPENCL

class OpenCLAdapter {
protected:
  void allocDevices(cl_device_type devType, unsigned devsCount);
  void clearDevices();

  cl::Buffer allocBuffer(size_t size);
  cl::CommandQueue allocQueue(unsigned dev);

  void compile(const std::string &dataDir, const std::string &file);
  cl::Kernel load(const std::string &name);

  size_t preferredWGSizeMultiple(cl::Kernel &kernel, unsigned dev);

private:
  std::string devTypeToString(cl_device_type devType);

private:
  cl::Platform _plat;
  cl::Context _ctx;
  std::vector<cl::Device> _devs;

  cl::Program _prog;
};

#endif // HAVE_OPENCL

} // End namespace florentino.

#endif // FLORENTINO_BENCHMARK_H
