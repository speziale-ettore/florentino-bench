
#include "benchmarks.h"

#include <sstream>
#include <stdexcept>

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
