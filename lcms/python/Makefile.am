#
# Makefile for building LCMS Python scripting extension
# Written by Bob Friesenhahn, June 2003
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

AM_CPPFLAGS =  -I$(top_builddir)/include -I$(top_srcdir)/include

pkgdir = $(LCMS_PYEXECDIR)

# Shared libraries built in this directory
if SUPPORT_PYTHON

pkg_LTLIBRARIES = _lcms.la
pkg_DATA = lcms.py

else
pkg_LTLIBRARIES =
pkg_DATA = 

endif

_lcms_la_LDFLAGS = -no-undefined -module -avoid-version \
                    -L$(LCMS_PYLIB)/config 

_lcms_la_LIBADD = $(top_builddir)/src/liblcms.la -lpython$(PYTHON_VERSION)

_lcms_la_SOURCES = lcms_wrap.cxx

_lcms_la_CPPFLAGS = -I$(PYTHON_PREFIX)/include/python$(PYTHON_VERSION) \
		    -I$(LCMS_PYINCLUDE) \
                    $(AM_CPPFLAGS)


EXTRA_DIST = lcms.py lcms.i testbed swig_lcms


clean-local:
	rm -f lcms.pyc

