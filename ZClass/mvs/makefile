#######################################################################
#                                                                     #
# MAKEFILE  : <                >                                      #
# AUTHOR    : <                >                                      #
# CREATED   : ../../....                                              #
#                                                                     #
#######################################################################
#
AR  = ar
RM  = rm
CC  = cc
CXX = c++
CXXFLAGS =
CXXOPT   = -W 0,"LANGLVL(EXTENDED) CSECT" -O -D_ALL_SOURCE -DNDEBUG
CFLAGS =
LIBPATHs= -L.
INCPATHs= -I. -I/u/stlport -I"//'SPAZIO.CSSLIB.H'"
LIBs= -l "//'SPAZIO.CSSLIB.OBJECT'" -l zclass

TARGET = TestZTrace
LIBTARGET = libzclass.a

FILE_H                = \
	zdefs.h       \
	zexcbase.h    \
	zexcept.h     \
	zmstrlck.h	\
	ztrace.h	\
	zreslock.h  

MODU_C             = \
	$(LIBTARGET)(zexcbase.cpp) \
	$(LIBTARGET)(zexcept.cpp)  \
	$(LIBTARGET)(zmstrlck.cpp)	\
	$(LIBTARGET)(ztrace.cpp)	\
	$(LIBTARGET)(zreslock.cpp) 

MODU_o            = $(subst .cpp,.o,$(MODU_C))

#
# Internal rule modify
#
.SUFFIXES :
.SUFFIXES : .o .cpp .a

.cpp.o :
	$(CXX) $(CXXOPT) $(INCPATHs) -c  $<

.cpp.a :
	$(CXX) $(CXXOPT) $(INCPATHs) -c  $<
	$(AR) -r $@ $*.o
	$(RM) -f $*.o



#
#
#
#all: $(LIBTARGET)
all: $(LIBTARGET) $(TARGET)

$(LIBTARGET):  $(MODU_o)

$(TARGET):  $(TARGET).o $(FILE_h)
	$(CXX)  -W l,p,map $(CXXOPT) $(INCPATHs) -o $@ $(LIBPATHs) $(TARGET).o \
				"//'SPAZIO.CSSLIB.OBJECT(BCSSDEQ)'" \
				"//'SPAZIO.CSSLIB.OBJECT(BCSSENQ)'" \
				"//'SPAZIO.CSSLIB.OBJECT(CSSGTCB)'" \
				"//'SPAZIO.CSSLIB.OBJECT(CSSGTOD)'" \
				"//'SPAZIO.CSSLIB.OBJECT(CSSGASN)'" \
				$(LIBs)

#				"//'SPAZIO.CSSLIB.OBJECT(BCSSDEQ)'" \
#				"//'SPAZIO.CSSLIB.OBJECT(BCSSENQ)'" \
#				"//'SPAZIO.CSSLIB.OBJECT(CSSGTCB)'" \
#				"//'SPAZIO.CSSLIB.OBJECT(CSSGTOD)'" \
#				"//'SPAZIO.CSSLIB.OBJECT(CSSGASN)'" \

$(TARGET).o : $(TARGET).cpp $(FILE_h) $(LIBTARGET)

#
# remove all executables targets
#
clean:
	-$(RM) $(LIBTARGET)
	-$(RM) $(TARGET)

#
# remove all executables targets and object files
#
clobber: clean
	-$(RM) $(TARGET).o

