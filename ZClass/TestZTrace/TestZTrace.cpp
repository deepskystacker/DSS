// TestZTrace.cpp : Used to test the ZTrace class
//
// $Workfile$
// $Revision$
// $Date$
// $Log$
//
//
// Test harness for class ZTrace 
// output expected is similar to:
/*
00000000 2000/10/26 14:58:06.326 001436 00000634 +main()(52)
00000001 2000/10/26 14:58:08.990 001436 00000634   >This is the first line,
00000001 2000/10/26 14:58:08.990 001436 00000634   >this is the second
00000001 2000/10/26 14:58:08.990 001436 00000634   >and this is the third
00000002 2000/10/26 14:58:09.000 001436 00000634   >Testing< file: C:\PROJECTS\ZCLass\TestZTrace\TestZTrace.cpp function: int main(int, char*) line: 55
00000003 2000/10/26 14:58:10.012 001436 00000634   >Windows NOTYET< file: C:\PROJECTS\ZCLass\TestZTrace\TestZTrace.cpp function: int main(int, char*) line: 56
00000004 2000/10/26 14:58:10.012 001436 00000634   >Windows NOP< file: C:\PROJECTS\ZCLass\TestZTrace\TestZTrace.cpp function: int main(int, char*) line: 57
00000005 2000/10/26 14:58:10.022 001436 00000634   +void subr(void)(82)
00000006 2000/10/26 14:58:10.022 001436 00000634     >Trace record from subr()
00000007 2000/10/26 14:58:10.022 001436 00000634   -void subr(void)
00000008 2000/10/26 14:58:10.032 001436 00000634   >ZAssertionFailure exception thrown.
00000008 2000/10/26 14:58:10.032 001436 00000634   >   function: int main(int, char*)
00000008 2000/10/26 14:58:10.032 001436 00000634   >   file: C:\PROJECTS\ZCLass\TestZTrace\TestZTrace.cpp
00000008 2000/10/26 14:58:10.032 001436 00000634   >   line: 61
00000009 2000/10/26 14:58:10.032 001436 00000634   >   Error Id is unavailable.
00000010 2000/10/26 14:58:10.042 001436 00000634   >   Error Code group is Other Error Group
00000011 2000/10/26 14:58:10.042 001436 00000634   >   Exception text is:
00000012 2000/10/26 14:58:10.052 001436 00000634   >      The following expression must be true, but evaluated to false: 0 == 1
2000/10/26 14:58:10.102 001436 00000634   >Trace entry without the line number in the prefix
  >Trace entry with no prefix
00000015 001436 00000634   >Trace entry without a timestamp in the prefix
00000016 2000/10/26 14:58:10.112 001436 00000634 -main()
*/
//

#if defined(__IBMCPP__) && defined(_AIX)
#pragma info(none)
#endif
#include <stdio.h>
#include <string>
#if defined(__IBMCPP__) && defined(_AIX)
#pragma info(restore)
#endif

#define Z_TRACE_DEVELOP
#include <zexcept.h>
#include <ztrace.h>

void subr(void);
int main(int argc, char* argv[])
{
  //
  // Set up the environment variable for the trace file
  //
#if defined(_WIN32)
  _putenv("Z_TRACEFILE=TestZTrace.txt");
  _putenv("Z_CHECKSTACK=ON");
#else
  putenv("Z_TRACEFILE=TestZTrace.txt");
#endif
  ZTrace::enableTrace();
  ZTrace::writeToFile();
  // ZMODTRACE_RUNTIME("main()");
  ZFUNCTRACE_RUNTIME();
	printf("Hello World!\n");
  ZTRACE_RUNTIME("This is the first line,\nthis is the second\nand this is the third");
  ZTrace::writeDebugLocation("Testing", ZEXCEPTION_LOCATION());
  ZTRACE_WIN_NOTYET();
  ZTRACE_WIN_NOP();
  subr();
  try
  {
    ZASSERTSTATE(0 == 1);
  }
  catch (ZException& e)
  {
    fprintf(stderr, "%s\n", e.text());
  }
  ZTrace::disableWriteLineNumber();
  ZTRACE_RUNTIME("Trace entry without the line number in the prefix");
  ZTrace::enableWriteLineNumber();
  ZTrace::disableWritePrefix();
  ZTRACE_RUNTIME("Trace entry with no prefix");
  ZTrace::enableWritePrefix();
  ZTrace::disableWriteTimeStamp();
  ZTRACE_RUNTIME("Trace entry without a timestamp in the prefix");
  ZTrace::enableWriteTimeStamp();
  return 0;
}
void subr()
{
  ZMODTRACE_DEVELOP("subr()");
  char aray[10000] = {0};
  ZTRACE_DEVELOP("Trace record from subr()");
}

#if defined(__IBMCPP__) && defined(_AIX)
#pragma info(none)
#endif
