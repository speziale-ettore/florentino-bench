
#include "benchmarks.h"

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

void StreamBench::check(const std::vector<double> &a,
                        const std::vector<double> &b,
                        const std::vector<double> &c,
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
