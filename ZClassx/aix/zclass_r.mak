CXX=xlC_r

ifdef NDEBUG
OUTDIR= ./release
CXXFLAGS= -DNDEBUG -O -I .
DEBPROD = production
else
OUTDIR = ./debug
CXXFLAGS= -g -I .
DEBPROD = debug
endif

LIBNAME = libzclass_r.a
TARGET=$(OUTDIR)/$(LIBNAME)

sources =\
	zexcbase.cpp zexcept.cpp zinttimr.cpp zmstrlck.cpp\
	zreslock.cpp ztrace.cpp

OBJS = $(OUTDIR)/zexcbase.o\
	$(OUTDIR)/zexcept.o\
	$(OUTDIR)/zinttimr.o\
	$(OUTDIR)/zmstrlck.o\
	$(OUTDIR)/zreslock.o\
	$(OUTDIR)/ztrace.o

all: delobjs message $(TARGET)

$(TARGET): $(OBJS)
	$(AR) -r $@ $?

$(OBJS): $(OUTDIR)%.o: .%.cpp
	$(CXX) $(CXXFLAGS) $(INCPATHS) -o $(OUTDIR)$*.o -c .$*.cpp

clean:
	-rm -f tempinc/* 2>/dev/null
	-rm -f $(TARGET) 2>/dev/null
	-rm -f $(OBJS) 2>/dev/null

delobjs:
	-rm -f tempinc/* 2>/dev/null
	-rm -f $(OBJS) 2>/dev/null

message:
	@echo "Building $(DEBPROD) version of $(LIBNAME) in directory $(OUTDIR)"
