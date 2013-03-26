
#ifndef FLORENTINO_MEMORY_H
#define FLORENTINO_MEMORY_H

#include <cstdlib>

namespace florentino {

inline void *xalloc(size_t size) {
  void *addr = malloc(size);

  assert(addr && "memory allocation failed");

  return addr;
}

inline void *xcalloc(size_t n, size_t size) {
  void *addr = calloc(n, size);

  assert(addr && "memory allocation failed");

  return addr;
}

inline void *xacalloc(size_t n, size_t size, size_t align) {
  void *addr;

  posix_memalign(&addr, align, n * size);

  assert(addr && "memory allocation failed");

  memset(addr, 0, n * size);

  return addr;
}

inline void xfree(void *addr) {
  free(addr);
}

template <typename Ty>
inline Ty *xalloc() {
  return reinterpret_cast<Ty *>(xalloc(sizeof(Ty)));
}

template <typename Ty>
inline Ty *xcalloc(size_t n) {
  return reinterpret_cast<Ty *>(xcalloc(n, sizeof(Ty)));
}

template <typename Ty>
inline Ty *xacalloc(size_t n, size_t align) {
  return reinterpret_cast<Ty *>(xacalloc(n, sizeof(Ty), align));
}

} // End namespace florentino.

#endif // FLORENTINO_MEMORY_H
