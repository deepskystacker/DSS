#
# Top-Level Makefile for building LCMS
# Written by Bob Friesenhahn, June 2003
#

# Don't require all the GNU mandated files
AUTOMAKE_OPTIONS = 1.7.2 dist-zip dist-bzip2 foreign

PACKAGE_NAME = @PACKAGE_NAME@
PACKAGE_VERSION = @PACKAGE_VERSION@

# Directories containing Makefiles to 'make'
SUBDIRS = src jpegicc tifficc python samples testbed include

# Additional files to distribute
EXTRA_DIST = AUTHORS COPYING ChangeLog doc INSTALL NEWS README.1ST \
	filter.c install.gcc lcms.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = lcms.pc

