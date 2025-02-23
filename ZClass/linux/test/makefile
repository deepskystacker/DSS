CXX=xlC_r7
CC=xlC_r7
CXXFLAGS=-I $(HOME)/zclass 
LDFLAGS=-I /$(HOME)/zclass -L ../release -lzclass_r

sources = TestZTrace.cpp
objs = $(subst .cpp,.o,$(sources))

TestZTrace: $(objs)

clean:
	@-rm *.o >/dev/null 2>&1
	@-rm TestZTrace >/dev/null 2>&1
	@-rm TestZTrace.txt >/dev/null 2>&1
