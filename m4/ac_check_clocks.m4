
dnl: ac_check_clocks.m4: check for functions for reading clocks.

AC_DEFUN([AC_CHECK_CLOCKS],
[

AC_CHECK_FUNC([clock_gettime])

AS_IF([test "x$ac_cv_func_clock_gettime" != "xyes"],
      [AC_CHECK_LIB([rt], [clock_gettime])])

])
