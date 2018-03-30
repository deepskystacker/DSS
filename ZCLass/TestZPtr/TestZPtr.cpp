//
// $Workfile$
// $Revision$
// $Date$
// $Log$
//
//   Rev 1.0   Jul 20 10:54:34 1998   DPartrid
// 
//
// Test harness for zptr.h class ZMngPointer reference counted pointer class
// output expected is similar to:
//
//    p2 == p3 is 0
//    p2 == p3 is 1
//    p2 == p4 is 0
//    p2 != p4 is 1
//    The value of the int via p1 is 10
//    The value of the int via p2 is 10
//    The value of the int via *pIntPtr is 10
//    p1 == p2 is 1
//    p1 == (int*)p1 is 1
//    p1 != (int*)p1 is 0
//    The address of the int is 00470E40
//    The value of the int via p1 is 10
//    Content of Xtest Structure is {15,a}
//

#if defined(_MSC_VER)
#pragma warning(disable : 4710)  // Function not inlined
#endif

#if defined(_MSC_VER)
#include <xstddef>               // Need to include this before disabling warnings
/////////////////////////////////////////////////////////////////////////////
// Turn off warnings for /W4
// To resume any of these warning: #pragma warning(default: 4xxx)
// which should be placed after the Standard C++ include files
#ifndef ALL_WARNINGS
// warnings generated with Standard C++ include files
#pragma warning(disable : 4018)  // signed/unsigned mismatch
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4146)  // unary minus applied to unsigned
#pragma warning(disable : 4244)  // loss of precision on conversion
#pragma warning(disable : 4245)  // conversion signed/unsigned mismatch
#pragma warning(disable : 4511)  // copy ctor could not be generated
#pragma warning(disable : 4512)  // assignment operator could not be generated
#pragma warning(disable : 4663)  // C++ template specialisation change
#endif //!ALL_WARNINGS
#endif //_MSC_VER

#include <cassert>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////
// Turn warnings back on for /W4
#ifndef ALL_WARNINGS
// warnings generated with Standard C++ include files
#pragma warning(default : 4018)  // signed/unsigned mismatch
#pragma warning(default : 4100)  // unreferenced formal parameter
#pragma warning(default : 4146)  // unary minus applied to unsigned
#pragma warning(default : 4244)  // loss of precision on conversion
#pragma warning(default : 4245)  // conversion signed/unsigned mismatch
#pragma warning(default : 4511)  // copy ctor could not be generated
#pragma warning(default : 4512)  // assignment operator could not be generated
#pragma warning(default : 4663)  // C++ template specialisation change
#endif //!ALL_WARNINGS

#include "zptr.h"

typedef ZMngPointer<int> IntPtr;
typedef struct x_test
{
  int anInt;
  char aChar;
} XTEST;
typedef ZMngPointer<XTEST> MPXTEST;

int main (int argc, char * argv [])
{
  argc; argv; // Reference parms


  //
  // Create a managed pointer to an int
  //
  IntPtr p1 (new int);    
  *p1 = 10;     // set its value to 10
  {
    IntPtr p2(p1);
    IntPtr * pIntPtr = new IntPtr(p2); // Set up another managed pointer to the int
    IntPtr p3;
    std::cout << "p2 == p3 is " << bool(p2 == p3) << std::endl;
    p3 = p2;
    std::cout << "p2 == p3 is " << bool(p2 == p3) << std::endl;  


    IntPtr p4 (new int); *p4 = 20;
    std::cout << "p2 == p4 is " << bool(p2 == p4) << std::endl;
    std::cout << "p2 != p4 is " << bool(p2 != p4) << std::endl;

    std::cout << "The value of the int via p1 is " << *p1 << std::endl;
    std::cout << "The value of the int via p2 is " << *p2 << std::endl;
    std::cout << "The value of the int via *pIntPtr is " << **pIntPtr << std::endl;
    delete pIntPtr;
    std::cout << "p1 == p2 is " << bool(p1 == p2) << std::endl;

  }
  std::cout << "p1 == (int*)p1 is " << bool(p1 == (int*)p1) << std::endl;
  std::cout << "p1 != (int*)p1 is " << bool(p1 != (int*)p1) << std::endl;
  std::cout << "The address of the int is " << (void*)(int*)p1 << std::endl;
  std::cout << "The value of the int via p1 is " << *p1 << std::endl;

  //
  // Now we've tested the basic function with a simple data type, ensure
  // that the operator -> () mf works when dealing with a more complex 
  // data type.
  //
  {
    MPXTEST mpXTest(new XTEST);
    mpXTest->anInt = 15;
    mpXTest->aChar = 'a';

    std::cout << "Content of Xtest Structure is {" 
              << mpXTest->anInt 
              << ","
              << mpXTest->aChar
              << "}"
              << std::endl;
  }
  
  return 0;
}
#if defined(_MSC_VER)
#ifndef ALL_WARNINGS
// warnings generated with Standard C++ include files
#pragma warning(disable : 4018)  // signed/unsigned mismatch
#pragma warning(disable : 4100)  // unreferenced formal parameter
#pragma warning(disable : 4146)  // unary minus applied to unsigned
#pragma warning(disable : 4244)  // loss of precision on conversion
#pragma warning(disable : 4245)  // conversion signed/unsigned mismatch
#pragma warning(disable : 4511)  // copy ctor could not be generated
#pragma warning(disable : 4512)  // assignment operator could not be generated
#pragma warning(disable : 4663)  // C++ template specialisation change
#endif //!ALL_WARNINGS
#endif //_MSC_VER

