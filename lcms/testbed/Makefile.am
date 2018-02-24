#
# Makefile for building testcms
# Written by Bob Friesenhahn, June 2003
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

INCLUDES	=  -I$(top_builddir)/include -I$(top_srcdir)/include

check_PROGRAMS = testcms

testcms_LDADD = $(top_builddir)/src/liblcms.la 
testcms_LDFLAGS = @LDFLAGS@
testcms_SOURCES = testcms.c

EXTRA_DIST = sRGBSpac.icm

check:
	./testcms

dist-hook:
	cp -p $(srcdir)/"sRGB Color Space Profile.icm" $(distdir)
