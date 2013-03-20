
dnl configure.ac: project autoconfiguration script config file.

AC_PREREQ([2.64])

AC_INIT([florentino-bench], [0.1], [speziale.ettore@gmail.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE()

AC_PROG_CXX()
AC_PROG_LIBTOOL()

AC_LANG([C++])

AC_CHECK_OPENCL()
AC_CHECK_CLOCKS()

AC_CONFIG_FILES([Makefile \
                 include/Makefile \
                 src/Makefile \
                 src/florentino/Makefile \
                 src/stream/Makefile])

AC_OUTPUT()
