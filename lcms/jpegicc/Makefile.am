#
# Makefile for building jpegicc
# Written by Bob Friesenhahn, June 2003
# Bugs introduced by Marti Maria on October 2004

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

AM_CPPFLAGS = -I$(top_builddir)/include -I$(top_srcdir)/include

if HasJPEG
bin_PROGRAMS = jpegicc
else
bin_PROGRAMS = 
endif

jpegicc_LDADD = $(top_builddir)/src/liblcms.la @JPEGICC_DEPLIBS@
jpegicc_LDFLAGS = @LDFLAGS@
jpegicc_SOURCES = jpegicc.c iccjpeg.c ../samples/xgetopt.c ../samples/vprf.c
man_MANS = jpegicc.1

EXTRA_DIST = iccjpeg.h $(man_MANS)
