#
# Makefile for libzclass_r
#
# Use make NDEBUG=1 for a production build to the ./release
# directory.
#
# Use make for a debug build to the ./debug directory
#

ifdef NDEBUG
OUTDIR=./release
CXXFLAGS=-O3 -I . -D_GNU_SOURCE -D_THREAD_SAFE -DNDEBUG -fPIC -W -Wall
DEBPROD=production
else
OUTDIR=./debug
CXXFLAGS=-g -I . -D_GNU_SOURCE -D_THREAD_SAFE -fPIC -W -Wall
DEBPROD=debug
endif

LIBNAME = libzclass_r.a
TARGET=$(OUTDIR)/$(LIBNAME)

sources = zexcbase.cpp zexcept.cpp zinttimr.cpp zmstrlck.cpp zreslock.cpp ztrace.cpp

objs=$(sources:%.cpp=$(OUTDIR)/%.o)

all: zclass_r.mak $(OUTDIR) message $(TARGET)

$(TARGET): $(objs)
	$(AR) crv $@ $?
	-rm -f $(objs) 2>/dev/null

$(OUTDIR):
	mkdir $(OUTDIR)

$(OUTDIR)/%.o: %.cpp
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

clean:
	-rm -f tempinc/* 2>/dev/null
	-rm -f $(TARGET) 2>/dev/null
	-rm -f $(objs) 2>/dev/null

message:
	@echo "Building $(DEBPROD) version of $(LIBNAME) in directory $(OUTDIR)"

