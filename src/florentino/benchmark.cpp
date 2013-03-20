
#include "florentino/benchmark-runner.h"

using namespace florentino;

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
  const TimeStat &stat = _clocks[1] - _clocks[0];

  for(tks_iterator i = stat.begin(), e = stat.end(); i != e; ++i)
    log() << " " << std::scientific << *i;
  log() << " " << std::scientific << stat.avg();
}
