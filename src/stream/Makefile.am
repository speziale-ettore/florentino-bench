
## dnl Makefile.am: build stream benchmark.

MAINTAINERCLEANFILES = Makefile.in

bin_PROGRAMS = florentino-stream

florentino_stream_CPPFLAGS = -I$(top_srcdir)/include
florentino_stream_SOURCES = florentino-stream.cpp \
                            benchmarks.h benchmarks.cpp \
                            cpu-stream.h cpu-stream.cpp
florentino_stream_LDADD = $(top_builddir)/src/florentino/libflorentino.la
