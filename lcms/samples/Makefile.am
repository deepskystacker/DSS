#
# Makefile for building lcms sample programs
# Originally Written by Bob Friesenhahn, June 2003
# Additions and bugs by Marti Maria Oct 2004

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7 foreign

AM_CPPFLAGS =  -I$(top_builddir)/include -I$(top_srcdir)/include

bin_PROGRAMS = icctrans wtpt icc2ps icclink

icctrans_LDADD = $(top_builddir)/src/liblcms.la 
icctrans_LDFLAGS = @LDFLAGS@
icctrans_SOURCES = icctrans.c xgetopt.c vprf.c
icctrans_MANS = icctrans.1

wtpt_LDADD = $(top_builddir)/src/liblcms.la 
wtpt_LDFLAGS = @LDFLAGS@
wtpt_SOURCES = wtpt.c xgetopt.c 


icc2ps_LDADD = $(top_builddir)/src/liblcms.la 
icc2ps_LDFLAGS = @LDFLAGS@
icc2ps_SOURCES = icc2ps.c xgetopt.c 

icclink_LDADD = $(top_builddir)/src/liblcms.la 
icclink_LDFLAGS = @LDFLAGS@
icclink_SOURCES = icclink.c xgetopt.c vprf.c

man_MANS  = wtpt.1 icc2ps.1 icclink.1
EXTRA_DIST = $(man_MANS)
