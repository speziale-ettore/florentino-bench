
#include "florentino/benchmark-runner.h"

#include <iostream>
#include <sstream>
#include <stdexcept>

#include <cstdlib>

using namespace florentino;

namespace {

void timesHandler(void *arg, const char *optArg) {
  unsigned int *times = reinterpret_cast<unsigned int *>(arg);

  // Parse into a signed type to get detected whether the used specified a
  // negative number.
  int value;

  std::istringstream is(optArg);
  is >> value;

  if(is.fail() || !is.eof()) {
    std::ostringstream os;
    os << "Error: option '-r' expects a positive number, "
          "got '" << optArg << "'";

    throw std::runtime_error(os.str());
  }

  if(value < 1)
    throw std::runtime_error("Error: option '-r' expects a positive number");

  *times = value;
}

void verboseHandler(void *arg, const char *optArg) {
  bool *verbose = reinterpret_cast<bool *>(arg);

  *verbose = true;
}

} // End anonymous namespace.

//
// BenchmarkRunner implementation.
//

BenchmarkRunner::BenchmarkRunner(int argc, char **argv) : _options(argc, argv),
                                                          _times(DEFAULT_TIMES),
                                                          _verbose(DEFAULT_VERBOSE) {
  _options.add(Option('r', Option::REQUIRED_ARGUMENT,
                      timesHandler, &_times,
                      "-r R", "repeat benchmark R times"));
  _options.add(Option('v', Option::NO_ARGUMENT,
                      verboseHandler, &_verbose,
                      "-v", "enable verbose output"));
}

BenchmarkRunner::~BenchmarkRunner() {
  typedef std::vector<Benchmark *>::iterator iterator;

  for(iterator i = _benchmarks.begin(), e = _benchmarks.end(); i != e; ++i)
    delete *i;
}

int BenchmarkRunner::run() {
  typedef std::vector<Benchmark *>::iterator iterator;

  try {
    _options.parse();

    for(iterator i = _benchmarks.begin(), e = _benchmarks.end(); i != e; ++i) {
      Benchmark *bench = *i;

      // Execute the benchmark.
      try {
        bench->setup();
        for(unsigned j = 0, f = _times; j != f; ++j) {
          bench->run();
        }
        bench->teardown();

      // Intercept any error that occurs in the benchmark and give him a last
      // chance to teardown benchmarking environment.
      } catch(const std::exception &ex) {
        bench->teardown();
        throw ex;
      }
    }

    return EXIT_SUCCESS;

  } catch(const std::exception &ex) {
    std::cerr << ex.what() << std::endl;
    return EXIT_FAILURE;
  }
}
