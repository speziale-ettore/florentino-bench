
#include "cpu-stream.h"

#include "florentino/memory.h"

using namespace florentino;

void CPUStream::setup() {
  _a = xcalloc<double>(arrayLength());
  _b = xcalloc<double>(arrayLength());
  _c = xcalloc<double>(arrayLength());

  StreamBench::setup();
}

void CPUStream::teardown() {
  StreamBench::teardown();

  xfree(_a);
  xfree(_b);
  xfree(_c);
}

void CPUStream::init() {
  for(unsigned i = 0, e = arrayLength(); i != e; ++i) {
    _a[i] = 1.0;
    _b[i] = 2.0;
    _c[i] = 0.0;
    _a[i] *= 2.0;
  }
}

void CPUStream::copy() {
  for(unsigned i = 0, e = arrayLength(); i != e; ++i)
    _c[i] = _a[i];
}

void CPUStream::scale(double k) {
  // Actually k is a constant, but in the original benchmark it is stored in a
  // variable -- probably the original author was interested in understanding
  // whether the compiler is smart enough ...
  for(unsigned i = 0, e = arrayLength(); i != e; ++i)
    _b[i] = k * _c[i];
}

void CPUStream::add() {
  for(unsigned i = 0, e = arrayLength(); i != e; ++i)
    _c[i] = _a[i] + _b[i];
}

void CPUStream::triad(double k) {
  // See comment on CPUStream::scale.
  for(unsigned i = 0, e = arrayLength(); i != e; ++i)
    _a[i] = _b[i] + k * _c[i];
}

void CPUStream::check(double k) {
  StreamBench::check(&_a[0], &_b[0], &_c[0], k);
}
