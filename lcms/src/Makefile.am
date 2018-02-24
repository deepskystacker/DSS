#
# Makefile for building LCMS library
# Initially Written by Bob Friesenhahn, June 2003
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

includedir = ${prefix}/include

# Shared libraries built in this directory
lib_LTLIBRARIES = liblcms.la

LIBRARY_CURRENT    = @LIBRARY_CURRENT@
LIBRARY_REVISION   = @LIBRARY_REVISION@
LIBRARY_AGE        = @LIBRARY_AGE@

INCLUDES = -I$(top_builddir)/include -I$(top_srcdir)/include

liblcms_la_LDFLAGS = -no-undefined \
  -version-info $(LIBRARY_CURRENT):$(LIBRARY_REVISION):$(LIBRARY_AGE)

liblcms_la_LIBADD = $(LCMS_LIB_DEPLIBS)

# liblcms_la_CFLAGS = -O3

liblcms_la_SOURCES = \
  cmscnvrt.c cmserr.c cmsgamma.c cmsgmt.c cmsintrp.c cmsio0.c cmsio1.c cmslut.c \
  cmsmatsh.c cmsmtrx.c cmspack.c cmspcs.c cmswtpnt.c cmsxform.c \
  cmssamp.c cmscam97.c cmsnamed.c cmsps2.c cmscam02.c cmsvirt.c cmscgats.c

