
#include "cpu-stream.h"

#include <iomanip>

using namespace florentino;

namespace {

std::ostream &hline(std::ostream &os) {
  for(unsigned i = 0, e = 62; i != e; ++i)
    os << "-";
  os << std::endl;

  return os;
}

} // End anonymonus namespace.

void CPUStream::setup() {
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

void CPUStream::run() {
  copy();
  scale(3.0);
  add();
  triad(3.0);
}

void CPUStream::teardown() {
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

void CPUStream::init() {
  for(unsigned i = 0, e = _arrayLength; i != e; ++i) {
    _a[i] = 1.0;
    _b[i] = 2.0;
    _c[i] = 0.0;
    _a[i] *= 2.0;
  }
}

void CPUStream::copy() {
  for(unsigned i = 0, e = _arrayLength; i != e; ++i)
    _c[i] = _a[i];
}

void CPUStream::scale(double k) {
  // Actually k is a constant, but in the original benchmark it is stored in a
  // variable -- probably the original author was interested in understanding
  // whether the compiler is smart enough ...
  for(unsigned i = 0, e = _arrayLength; i != e; ++i)
    _b[i] = k * _c[i];
}

void CPUStream::add() {
  for(unsigned i = 0, e = _arrayLength; i != e; ++i)
    _c[i] = _a[i] + _b[i];
}

void CPUStream::triad(double k) {
  // See comment on CPUStream::scale.
  for(unsigned i = 0, e = _arrayLength; i != e; ++i)
    _a[i] = _b[i] + k * _c[i];
}

void CPUStream::check(double k) const {
  StreamBench::check(_a, _b, _c, 3.0);
}
