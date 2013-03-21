
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

std::ostream &hline(std::ostream &os) {
  for(unsigned i = 0, e = 62; i != e; ++i)
    os << "-";
  os << std::endl;

  return os;
}

} // End anonymous namespace.

//
// StreamBenchmarkRunner implementation.
//

StreamBenchmarkRunner::StreamBenchmarkRunner(int argc, char *argv[])
  : BenchmarkRunner(argc, argv),
    _arrayLength(DEFAULT_ARRAY_LENGTH) {
  add(Option('l', Option::REQUIRED_ARGUMENT,
             arrayLengthHandler, &_arrayLength,
             "-l L", "set array length to L"));
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

        << "Array size = " << _arrayLength
        << std::endl
        << "Total memory required = "
        << std::scientific << std::setprecision(1)
        << (3 * sizeof(double) * _arrayLength * 1e-6)
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
  double totalTime = _clocks[ClkEnd][runs() - 1] -
                     _clocks[ClkStart][runs() - 1];

  log() << "Average rate (MB/s): "
        << std::scientific << std::setprecision(4) << std::setw(11)
        << (3 * sizeof(double) * _arrayLength * runs() * 1e-6 / totalTime)
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
                        double k) const {
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
  ai *= _arrayLength;
  bi *= _arrayLength;
  ci *= _arrayLength;

  double aSum = 0.0,
         bSum = 0.0,
         cSum = 0.0;

  for(unsigned i = 0, e = _arrayLength; i != e; ++i) {
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
