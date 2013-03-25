
#ifdef cl_khr_fp64
#pragma OPENCL EXTENSION cl_khr_fp64 : enable
#else
#error "double precision floating point not supported by OpenCL implementation"
#endif

// NOTE: I prefered guarding all benchmarks with a loop rather than with a
// conditional, just in the case I want to increase the number of iterations
// performed by each kernel.

kernel void gpu_init(global double * restrict a,
                     global double * restrict b,
                     global double * restrict c,
                     uint n)
{
  uint stride = get_global_size(0);

  for(uint i = get_global_id(0); i < n; i += stride) {
    a[i] = 1.0;
    b[i] = 2.0;
    c[i] = 0.0;
    a[i] *= 2.0E0 * a[i];
  }
}

kernel void gpu_copy(global double * restrict a,
                     global double * restrict c,
                     uint n)
{
  uint stride = get_global_size(0);

  for(uint i = get_global_id(0); i < n; i += stride)
    c[i] = a[i];
}

kernel void gpu_scale(global double * restrict b,
                      global double * restrict c,
                      double k,
                      uint n)
{
  uint stride = get_global_size(0);

  for(uint i = get_global_id(0); i < n; i += stride)
    b[i] = k * c[i];
}

kernel void gpu_add(global double * restrict a,
                    global double * restrict b,
                    global double * restrict c,
                    uint n)
{
  uint stride = get_global_size(0);

  for(uint i = get_global_id(0); i < n; i += stride)
    c[i] = a[i] + b[i];
}

kernel void gpu_triad(global double * restrict a,
                      global double * restrict b,
                      global double * restrict c,
                      double k,
                      uint n)
{
  uint stride = get_global_size(0);

  for(uint i = get_global_id(0); i < n; i += stride)
    a[i] = b[i] + k * c[i];
}
