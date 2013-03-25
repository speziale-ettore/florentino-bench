
dnl: ac_check_opencl.m4: check for OpenCL libraries.

AC_DEFUN([AC_CHECK_OPENCL],
[

AC_CHECK_HEADERS([CL/cl.hpp], [ac_check_have_cl_hpp=yes])
AC_CHECK_LIB([OpenCL], [clGetPlatformIDs], [ac_check_have_libopencl=yes])

AS_IF([test "x$ac_check_have_cl_hpp" = "xyes" -a \
            "x$ac_check_have_libopencl" = "xyes"],
      [AC_DEFINE([HAVE_OPENCL], [1])
       LIBS="-lOpenCL $LIBS"])

])
