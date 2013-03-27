
#include "cpu-stream.h"

#include "florentino/memory.h"

using namespace florentino;

void CPUStream::teardown() {
  StreamBench::teardown();

  xfree(_a);
  xfree(_b);
  xfree(_c);
}

void CPUStream::check(double k) {
  StreamBench::check(_a, _b, _c, k);
}

// With sse2 we can vectorize operations using vectors of 2 doubles.
#if defined(__SSE2__)

#include <emmintrin.h>

#define KERNEL(K, E)               \
  unsigned i = 0,                  \
           e = arrayLength() & ~1; \
                                   \
  for(; i != e; i += 2) { K }      \
  if(i != arrayLength()) { E }

void CPUStream::setup() {
  _a = xacalloc<double>(arrayLength(), __alignof__(__v2df));
  _b = xacalloc<double>(arrayLength(), __alignof__(__v2df));
  _c = xacalloc<double>(arrayLength(), __alignof__(__v2df));

  StreamBench::setup();
}

void CPUStream::init() {
  KERNEL(
  {
    // _a[i] = 1.0;
    _mm_store_pd(_a + i, _mm_set1_pd(1.0));

    // _b[i] = 2.0;
    _mm_store_pd(_b + i, _mm_set1_pd(2.0));

    // _c[i] = 0.0;
    _mm_store_pd(_c + i, _mm_set1_pd(0.0));

    // _a[i] *= 2.0;
    _mm_store_pd(_a + i, _mm_mul_pd(_mm_set1_pd(2.0), _mm_load_pd(_a + i)));
  },
  {
    // _a[i] = 1.0;
    _mm_store_sd(_a + i, _mm_set_sd(1.0));

    // _b[i] = 2.0;
    _mm_store_sd(_b + i, _mm_set_sd(2.0));

    // _c[i] = 0.0;
    _mm_store_sd(_c + i, _mm_set_sd(0.0));

    // _a[i] *= 2.0;
    _mm_store_sd(_a + i, _mm_mul_sd(_mm_set_sd(2.0), _mm_load_sd(_a + i)));
  })
}
void CPUStream::copy() {
  KERNEL(
  {
    // _c[i] = _a[i];
    _mm_store_pd(_c + i, _mm_load_pd(_a + i));
  },
  {
    // _c[i] = _a[i];
    _mm_store_sd(_c + i, _mm_load_sd(_a + i));
  })
}

void CPUStream::scale(double k) {
  KERNEL(
  {
    // _b[i] = k * _c[i];
    _mm_store_pd(_b + i, _mm_mul_pd(_mm_set1_pd(k), _mm_load_pd(_c + i)));
  },
  {
    // _b[i] = k * _c[i];
    _mm_store_sd(_b + i, _mm_mul_sd(_mm_set_sd(k), _mm_load_sd(_c + i)));
  })
}

void CPUStream::add() {
  KERNEL(
  {
    //_c[i] = _a[i] + _b[i];
    _mm_store_pd(_c + i, _mm_add_pd(_mm_load_pd(_a + i), _mm_load_pd(_b + i)));
  },
  {
    //_c[i] = _a[i] + _b[i];
    _mm_store_sd(_c + i, _mm_add_sd(_mm_load_sd(_a + i), _mm_load_sd(_b + i)));
  })
}

void CPUStream::triad(double k) {
  KERNEL(
  {
    // _a[i] = _b[i] + k * _c[i];
    _mm_store_pd(_a + i,
                 _mm_add_pd(_mm_load_pd(_b + i),
                            _mm_mul_pd(_mm_set1_pd(k), _mm_load_pd(_c + i))));
  },
  {
    // _a[i] = _b[i] + k * _c[i];
    _mm_store_sd(_a + i,
                 _mm_add_sd(_mm_load_sd(_b + i),
                            _mm_mul_sd(_mm_set_sd(k), _mm_load_sd(_c + i))));
  })
}

// Normal scalar implementation. Performance will not be good, and it is
// unlikely the compiler can vectorize the code.
#else

void CPUStream::setup() {
  _a = xcalloc<double>(arrayLength());
  _b = xcalloc<double>(arrayLength());
  _c = xcalloc<double>(arrayLength());

  StreamBench::setup();
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

#endif // SCALAR_IMPLEMENTATION
