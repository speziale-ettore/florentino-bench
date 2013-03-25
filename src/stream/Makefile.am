
## dnl Makefile.am: build stream benchmark.

MAINTAINERCLEANFILES = Makefile.in

florentino_streamdir = $(pkgdatadir)

AM_CXXFLAGS = -DPACKAGE_DATADIR=\"$(florentino_streamdir)\"

bin_PROGRAMS = florentino-stream

florentino_stream_CPPFLAGS = -I$(top_srcdir)/include
florentino_stream_SOURCES = florentino-stream.cpp \
                            benchmarks.h benchmarks.cpp \
                            cpu-stream.h cpu-stream.cpp \
                            ocl-stream.h ocl-stream.cpp
florentino_stream_LDADD = $(top_builddir)/src/florentino/libflorentino.la
florentino_stream_DATA = florentino-stream-kernels.cl
