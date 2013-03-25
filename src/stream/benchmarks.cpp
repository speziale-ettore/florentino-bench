
#include "benchmarks.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <cmath>

using namespace florentino;

namespace {

void arrayLengthHandler(void *arg, const char *optArg) {
  unsigned int *arrayLength = reinterpret_cast<unsigned int *>(arg);

  // Parse to signed type to prevent negative sizes.
  int value;

  std::istringstream is(optArg);
  is >> value;

  if(is.fail() || !is.eof()) {
    std::ostringstream os;
    os << "Error: option '-l' expects a positive number, "
          "got '" << optArg << "'";

    throw std::runtime_error(os.str());
  }

  if(value < 1)
    throw std::runtime_error("Error: option '-l' expects a positive number");

  *arrayLength = value;
}

void devsCountHandler(void *arg, const char *optArg) {
  unsigned int *devsCount = reinterpret_cast<unsigned int *>(arg);

  // Parse to signed type to prevent negative sizes.
  int value;

  std::istringstream is(optArg);
  is >> value;

  if(is.fail() || !is.eof()) {
    std::ostringstream os;
    os << "Error: option '-c' expects a positive number, "
          "got '" << optArg << "'";

    throw std::runtime_error(os.str());
  }

  if(value < 1)
    throw std::runtime_error("Error: option '-c' expects a positive number");

  *devsCount = value;
}

void dataDirHandler(void *arg, const char *optArg) {
  std::string *dataDir = reinterpret_cast<std::string *>(arg);

  // Nothing to parse here. Defer path validation at the point where it is
  // actually used.
  *dataDir = optArg;
}

} // End anonymous namespace.

//
// StreamBenchmarkRunner implementation.
//

const size_t StreamBenchmarkRunner::DEFAULT_ARRAY_LENGTH
  = 24 / sizeof(double) * size_t(1e6);
const size_t StreamBenchmarkRunner::DEFAULT_DEVS_COUNT
  = 1;
const std::string StreamBenchmarkRunner::DEFAULT_DATA_DIR
  = PACKAGE_DATADIR;

StreamBenchmarkRunner::StreamBenchmarkRunner(int argc, char *argv[])
  : BenchmarkRunner(argc, argv),
    _arrayLength(DEFAULT_ARRAY_LENGTH),
    _devsCount(DEFAULT_DEVS_COUNT),
    _dataDir(DEFAULT_DATA_DIR) {
  add(Option('l', Option::REQUIRED_ARGUMENT,
             arrayLengthHandler, &_arrayLength,
             "-l L", "set array length to L"));
  add(Option('c', Option::REQUIRED_ARGUMENT,
             devsCountHandler, &_devsCount,
             "-c C", "use up to C OpenCL devices"));
  add(Option('d', Option::REQUIRED_ARGUMENT,
             dataDirHandler, &_dataDir,
             "-d D", "set data directory to D"));
}

//
// StreamBench implementation.
//

void StreamBench::setup() {
  log() << hline

        << "STREAM version $Revision BSC-5.8 $"
        << std::endl

        << hline

        << "This system uses "
        << sizeof(double)
        << " bytes per DOUBLE PRECISION word."
        << std::endl

        << hline

        << "Array size = " << arrayLength()
        << std::endl
        << "Total memory required = "
        << std::scientific << std::setprecision(1)
        << (3 * sizeof(double) * arrayLength() * 1e-6)
        << " MB."
        << std::endl

        << hline;

  // Initialize arrays.
  init();

  // Cold run.
  copy();
  scale(3.0);
  add();
  triad(3.0);

  // Re-initialize.
  init();
}

void StreamBench::run() {
  copy();
  scale(3.0);
  add();
  triad(3.0);
}

void StreamBench::teardown() {
  size_t memOpsPerIter = 2 + // copy: reads c[i] -- writes a[i]
                         2 + // scale: reads c[i] -- writes b[i]
                         3 + // add: reads a[i], b[i] -- writes c[i]
                         3;  // triad: reads b[i], c[i] -- writes a[i]

  double totalTime = _clocks[ClkEnd][runs() - 1] -
                     _clocks[ClkStart][runs() - 1];
  size_t totalSize = memOpsPerIter * arrayLength() * runs();

  log() << "Average rate (MB/s): "
        << std::scientific << std::setprecision(4) << std::setw(11)
        << (totalSize * 1e-6 / totalTime)
        << std::endl

        << "TOTAL time (without initialization) = "
        << std::scientific << std::setprecision(4) << std::setw(11)
        << totalTime
        << " seconds"
        << std::endl

        << hline;

   check(3.0);

   log() << hline;
}

void StreamBench::check(const double *a,
                        const double *b,
                        const double *c,
                        double k) {
  double ai, bi, ci;

  // Reproduce initialization.
  ai = 1.0;
  bi = 2.0;
  ci = 0.0;
  ai *= 2.0;

  // Simulate timed loop.
  for(unsigned i = 0, e = runs(); i != e; ++i) {
    ci = ai;
    bi = k * ci;
    ci = ai + bi;
    ai = bi + k * ci;
  }
  ai *= arrayLength();
  bi *= arrayLength();
  ci *= arrayLength();

  double aSum = 0.0,
         bSum = 0.0,
         cSum = 0.0;

  for(unsigned i = 0, e = arrayLength(); i != e; ++i) {
    aSum += a[i];
    bSum += b[i];
    cSum += c[i];
  }

  log() << "Result comparison:"
        << std::endl

        << "       expected  : "
        << std::scientific << ai << " "
        << std::scientific << bi << " "
        << std::scientific << ci << " "
        << std::endl

        << "       observed  : "
        << std::scientific << aSum << " "
        << std::scientific << bSum << " "
        << std::scientific << cSum << " "
        << std::endl;

  if(std::abs(ai - aSum) / aSum > 1e-8)
    throw std::runtime_error("Failed validation on array a[]");

  if(std::abs(bi - bSum) / bSum > 1e-8)
    throw std::runtime_error("Failed validation on array b[]");

  if(std::abs(ci - cSum) / cSum > 1e-8)
    throw std::runtime_error("Failed validation on array c[]");

  log() << "Solution validates" << std::endl;
}
